#include "expression.hpp"
#include "statement.hpp"
#include "token.hpp"
#include "utility.hpp"
#include <iostream>
#include <optional>
#include <variant>
#include <queue>
#include <stack>

namespace Scorpion {

	// program         -> declaration*
	// declaration     -> definestatement | expressionStmt

	// definestatement -> Define Symbol As typeid ( "=" expression )? Newline
	// expressionStmt  -> expression? Newline

	// expression      -> assignment
	// assignment      -> ( Symbol Equals assignment ) | equality
	// equality        -> comparison ( ( "!=" | "==" ) comparison )*
	// comparison      -> addition ( ( ">" | ">=" | "<" | "<=" ) addition )*
	// addition        -> multiplication ( ( "+" | "-" ) multiplication )*
	// multiplication  -> unary ( ( "/" | "*" ) unary )*
	// unary           -> ( ( Not | "-" ) unary ) | primary
	// primary         -> NumericLiteral | DecimalLiteral | StringLiteral
	//					  | False | True | Null | Symbol | "(" expression ")"

	// typeid          -> Symbol | ( TypeU8 | TypeS8 | ... )

	static std::stack< std::string > currentErrors;

	void logError( const FilePosition& position, const std::string& message ) {
		std::string base = "At " + std::to_string( position.row ) + ", " + std::to_string( position.column ) + ": ";

		currentErrors.push( base + message );
	}

	std::optional< Expression > getExpression( std::queue< Token >& tokens );

	Token takeAndPop( std::queue< Token >& tokens ) {
		Token copy = tokens.front();
		tokens.pop();
		return copy;
	}

	std::optional< Expression > getPrimary( std::queue< Token >& tokens ) {
		if( tokens.front().type == TokenType::NumericLiteral ||
			tokens.front().type == TokenType::DecimalLiteral ||
			tokens.front().type == TokenType::StringLiteral ||
			tokens.front().type == TokenType::False ||
			tokens.front().type == TokenType::True ||
			tokens.front().type == TokenType::Null ||
			tokens.front().type == TokenType::Symbol
		) {
			return Expression{ Literal{ takeAndPop( tokens ) } };
		}

		if( tokens.front().type == TokenType::LeftParen ) {
			// Eat leftparen
			tokens.pop();
			std::optional< Expression > expression = getExpression( tokens );
			if( expression ) {
				if( tokens.front().type == TokenType::RightParen ) {
					// Eat rightparen
					tokens.pop();
					return expression;
				} else {
					// Expected: right paren
					logError( tokens.front().position, "expected: right paren" );
					return {};
				}
			} else {
				// Expected: expression
				logError( tokens.front().position, "expected: expression" );
				return {};
			}
		}

		// Can't return a primary
		logError( tokens.front().position, "did not return a primary expression" );
		return {};
	}

	std::optional< Expression > getUnary( std::queue< Token >& tokens ) {
		if( tokens.front().type == TokenType::Not || tokens.front().type == TokenType::Minus ) {
			Token unaryOperator = takeAndPop( tokens );
			std::optional< Expression > unary = getUnary( tokens );
			if( unary ) {
				return UnaryExpression{
					std::make_unique< Expression >( std::move( *unary ) ),
					unaryOperator
				};
			} else {
				// Expected: unary
				logError( tokens.front().position, "expected: unary" );
				return {};
			}
		} else {
			return getPrimary( tokens );
		}
	}

	std::optional< Expression > getMultiplication( std::queue< Token >& tokens ) {
		std::optional< Expression > lhs = getUnary( tokens );

		if( !lhs ) {
			return lhs;
		}

		if( tokens.front().type == TokenType::ForwardSlash ||
			tokens.front().type == TokenType::Asterisk ) {
			Token binaryOperator = takeAndPop( tokens );
			std::optional< Expression > rhs = getUnary( tokens );
			if( rhs ) {
				return BinaryExpression {
					std::make_unique< Expression >( std::move( *lhs ) ),
					binaryOperator,
					std::make_unique< Expression >( std::move( *rhs ) )
				};
			} else {
				// Expected: unary
				logError( tokens.front().position, "expected: unary" );
				return {};
			}
		} else {
			return lhs;
		}
	}

	std::optional< Expression > getAddition( std::queue< Token >& tokens ) {
		std::optional< Expression > lhs = getMultiplication( tokens );

		if( !lhs ) {
			return lhs;
		}

		if( tokens.front().type == TokenType::Plus ||
			tokens.front().type == TokenType::Minus ) {
			Token binaryOperator = takeAndPop( tokens );
			std::optional< Expression > rhs = getMultiplication( tokens );
			if( rhs ) {
				return BinaryExpression {
					std::make_unique< Expression >( std::move( *lhs ) ),
					binaryOperator,
					std::make_unique< Expression >( std::move( *rhs ) )
				};
			} else {
				// Expected: multiplication
				logError( tokens.front().position, "expected: multiplication" );
				return {};
			}
		} else {
			return lhs;
		}
	}

	std::optional< Expression > getComparison( std::queue< Token >& tokens ) {
		std::optional< Expression > lhs = getAddition( tokens );

		if( !lhs ) {
			return lhs;
		}

		if( tokens.front().type == TokenType::GreaterThan ||
			tokens.front().type == TokenType::GreaterThanEqual ||
			tokens.front().type == TokenType::LessThan ||
			tokens.front().type == TokenType::LessThanEqual ) {
			Token binaryOperator = takeAndPop( tokens );
			std::optional< Expression > rhs = getAddition( tokens );
			if( rhs ) {
				return BinaryExpression {
					std::make_unique< Expression >( std::move( *lhs ) ),
					binaryOperator,
					std::make_unique< Expression >( std::move( *rhs ) )
				};
			} else {
				// Expected: addition
				logError( tokens.front().position, "expected: addition" );
				return {};
			}
		} else {
			return lhs;
		}
	}

	std::optional< Expression > getEquality( std::queue< Token >& tokens ) {
		std::optional< Expression > lhs = getComparison( tokens );

		if( !lhs ) {
			return lhs;
		}

		if( tokens.front().type == TokenType::EqualTo ||
			tokens.front().type == TokenType::NotEqualTo ) {
			Token binaryOperator = takeAndPop( tokens );
			std::optional< Expression > rhs = getComparison( tokens );
			if( rhs ) {
				return BinaryExpression {
					std::make_unique< Expression >( std::move( *lhs ) ),
					binaryOperator,
					std::make_unique< Expression >( std::move( *rhs ) )
				};
			} else {
				// Expected: comparison
				logError( tokens.front().position, "expected: comparison" );
				return {};
			}
		} else {
			return lhs;
		}
	}

	std::optional< Expression > getAssignment( std::queue< Token >& tokens ) {
		if( tokens.front().type == TokenType::Symbol ) {
			Token symbol = takeAndPop( tokens );
			if( tokens.front().type == TokenType::Equals ) {
				tokens.pop();	// Eat the equals, we just need to see if it's there
				std::optional< Expression > rhs = getAssignment( tokens );
				if( rhs ) {
					return AssignmentExpression{
						symbol,
						std::make_unique< Expression >( std::move( *rhs ) )
					};
				} else {
					// Expected: assignment
					logError( tokens.front().position, "expected: assignment" );
					return {};
				}
			} else {
				// Expected: equals
				logError( tokens.front().position, "expected: equals" );
				return {};
			}
		} else {
			return getEquality( tokens );
		}
	}

	std::optional< Expression > getExpression( std::queue< Token >& tokens ) {
		return getAssignment( tokens );
	}

	std::optional< ExpressionStatement > getExpressionStatement( std::queue< Token >& tokens ) {
		std::optional< Expression > expression = getExpression( tokens );

		if( tokens.front().type != TokenType::Newline ) {
			// Expected: newline
			logError( tokens.front().position, "expected: newline" );
			return {};
		} else {
			tokens.pop();
			return expression;
		}
	}

	std::optional< DefineStatement > getDefineStatement( std::queue< Token >& tokens ) {
		if( tokens.front().type == TokenType::Define ) {
			tokens.pop();
			if( tokens.front().type == TokenType::Symbol ) {
				Token symbol = takeAndPop( tokens );
				if( tokens.front().type == TokenType::As ) {
					tokens.pop();
					if( tokens.front().type == TokenType::Symbol ||
						tokens.front().type == TokenType::TypeU8 ||
						tokens.front().type == TokenType::TypeU16 ||
						tokens.front().type == TokenType::TypeU32 ||
						tokens.front().type == TokenType::TypeS8 ||
						tokens.front().type == TokenType::TypeS16 ||
						tokens.front().type == TokenType::TypeS32 ||
						tokens.front().type == TokenType::TypeReal ||
						tokens.front().type == TokenType::TypeString ||
						tokens.front().type == TokenType::TypeBoolean
					) {
						Token type = takeAndPop( tokens );

						if( tokens.front().type == TokenType::Equals ) {
							// Assignment expression
							tokens.pop();
							std::optional< Expression > expression = getExpression( tokens );
							if( expression ) {
								return DefineStatement{
									symbol,
									type,
									std::make_unique< Expression >( std::move( *expression ) )
								};
							} else {
								// Expected: expression
								logError( tokens.front().position, "expected: expression" );
								return {};
							}
						} else {
							// Default init
							return DefineStatement{ symbol, type, nullptr };
						}
					} else {
						// Expected: type
						logError( tokens.front().position, "expected: type" );
						return {};
					}
				} else {
					// Expected: as
					logError( tokens.front().position, "expected: as" );
					return {};
				}
			} else {
				// Expected: symbol
				logError( tokens.front().position, "expected: symbol" );
				return {};
			}
		} else {
			logError( tokens.front().position, "did not return a define statement" );
			return {};
		}
	}

	std::optional< Declaration > getDeclaration( std::queue< Token >& tokens ) {
		// One-token lookahead
		if( tokens.front().type == TokenType::Define ) {
			return getDefineStatement( tokens );
		} else {
			return getExpressionStatement( tokens );
		}
	}

	std::optional< Program > getProgram( std::queue< Token >& tokens ) {
		Program program;

		while( !tokens.empty() ) {
			std::optional< Declaration > declaration = getDeclaration( tokens );
			if( declaration ) {
				program.emplace_back( std::move( *declaration ) );
			} else {
				// Expected: declaration
				logError( tokens.front().position, "expected: declaration" );
				return {};
			}
		}

		return std::move( program );
	}

	std::variant< Program, std::stack< std::string > > compile( std::queue< Token >& tokens ) {
		std::optional< Program > program = getProgram( tokens );

		if( program ) {
			return std::move( *program );
		} else {
			return currentErrors;
		}
	}

	void printTree( const Program& program ) {
		std::cout << "Tree:" << std::endl;
		for( const Declaration& declaration : program ) {
			std::cout << toString( declaration ) << std::endl;
		}
	}

}