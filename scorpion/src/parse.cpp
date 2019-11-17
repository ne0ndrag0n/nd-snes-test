#include "expression.hpp"
#include "statement.hpp"
#include "token.hpp"
#include <optional>
#include <queue>

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
					return {};
				}
			} else {
				// Expected: expression
				return {};
			}
		}

		// Can't return a primary
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
					return {};
				}
			} else {
				// Expected: equals
				return {};
			}
		} else {
			return getEquality( tokens );
		}
	}

	std::optional< Expression > getExpression( std::queue< Token >& tokens ) {
		return getAssignment( tokens );
	}

	std::optional< ExpressionStatement > getExpressionStmt( std::queue< Token >& tokens ) {
		std::optional< Expression > expression = getExpression( tokens );

		if( tokens.front().type != TokenType::Newline ) {
			// Expected: newline
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
								return {};
							}
						} else {
							// Default init
							return DefineStatement{ symbol, type, nullptr };
						}
					} else {
						// Expected: type
						return {};
					}
				} else {
					// Expected: as
					return {};
				}
			} else {
				// Expected: symbol
				return {};
			}
		} else {
			return {};
		}
	}

}