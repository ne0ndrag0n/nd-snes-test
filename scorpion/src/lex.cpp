#include "token.hpp"
#include "utility.hpp"
#include <rang.hpp>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>

namespace Scorpion {

	static std::optional< char > getCharAt( const std::string& string, unsigned int index ) {
		if( index >= string.length() ) {
			return {};
		}

		return string[ index ];
	}

	static std::variant< Token, std::string > discernToken( const std::string& text, const FilePosition& position ) {
		std::variant< Token, std::string > result;

		Token token;
		token.position = position;
		switch( hash( text.c_str() ) ) {
			case hash( "def" ): {
				token.type = TokenType::Define;
				break;
			}
			case hash( "as" ): {
				token.type = TokenType::As;
				break;
			}
			case hash( "." ): {
				token.type = TokenType::Dot;
				break;
			}
			case hash( ">" ): {
				token.type = TokenType::GreaterThan;
				break;
			}
			case hash( ">=" ): {
				token.type = TokenType::GreaterThanEqual;
				break;
			}
			case hash( "<" ): {
				token.type = TokenType::LessThan;
				break;
			}
			case hash( "<=" ): {
				token.type = TokenType::LessThanEqual;
				break;
			}
			case hash( "==" ): {
				token.type = TokenType::EqualTo;
				break;
			}
			case hash( "!=" ): {
				token.type = TokenType::NotEqualTo;
				break;
			}
			case hash( "=" ): {
				token.type = TokenType::Equals;
				break;
			}
			case hash( "+" ): {
				token.type = TokenType::Plus;
				break;
			}
			case hash( "-" ): {
				token.type = TokenType::Minus;
				break;
			}
			case hash( "*" ): {
				token.type = TokenType::Asterisk;
				break;
			}
			case hash( "/" ): {
				token.type = TokenType::ForwardSlash;
				break;
			}
			case hash( "\\" ): {
				token.type = TokenType::BackwardSlash;
				break;
			}
			case hash( "(" ): {
				token.type = TokenType::LeftParen;
				break;
			}
			case hash( ")" ): {
				token.type = TokenType::RightParen;
				break;
			}
			case hash( "u8" ): {
				token.type = TokenType::TypeU8;
				break;
			}
			case hash( "u16" ): {
				token.type = TokenType::TypeU16;
				break;
			}
			case hash( "u32" ): {
				token.type = TokenType::TypeU32;
				break;
			}
			default: {
				// Token begins with a letter - symbol
				// Token begins with a number - numeric literal
				if( auto charResult = getCharAt( text, 0 ) ) {
					if(
						( *charResult >= 'a' && *charResult <= 'z' ) ||
						( *charResult >= 'A' && *charResult <= 'Z' ) ||
						*charResult == '_'
					 ) {
						 token.type = TokenType::Symbol;
						 token.value = text;
					} else if( *charResult >= '0' && *charResult <= '9' ) {
						// The entirety should be parseable as a number
						// If it can't then it shouldn't start with a number!
						size_t substring = text.find( "." );
						if( substring == std::string::npos ) {
							token.type = TokenType::NumericLiteral;
							try {
								token.value = std::stol( text );
							} catch( std::exception e ) {
								result = std::string( "Near " ) + std::to_string( position.row ) + "," + std::to_string( position.column ) + ": could not convert numeric literal to numeric type!";
								return result;
							}
						} else {
							token.type = TokenType::DecimalLiteral;
							try {
								token.value = std::stod( text );
							} catch( std::exception e ) {
								result = std::string( "Near " ) + std::to_string( position.row ) + "," + std::to_string( position.column ) + ": could not convert decimal literal to decimal type!";
								return result;
							}
						}
					} else {
						result = std::string( "Near " ) + std::to_string( position.row ) + "," + std::to_string( position.column ) + ": Invalid token!";
						return result;
					}
				}
			}
		}

		result = token;
		return result;
	}

	std::variant< std::queue< Token >, std::string > getTokens( const std::string& filename ) {
		std::variant< std::queue< Token >, std::string > result;

		std::ifstream file( filename );
		if( !file.is_open() ) {
			result = "Failed to open file!";
			return result;
		}
		std::queue< Token > tokens;

		std::string nextToken;
		FilePosition position;
		bool quit = false;
		while( !quit ) {
			int next = file.get();
			switch( next ) {
				case -1: {
					quit = true;
					[[ fallthrough ]];
				}
				case '\f':
				case '\r':
				case '\t':
				case '\v':
				case '\n':
				case ' ': {
					if( nextToken != "" ) {
						std::visit( overloaded {
							[ &tokens ]( Token token ) {
								tokens.push( token );
							},
							[]( const std::string& error ) {
								std::cout << rang::fg::red << "error: " << rang::style::reset << error << std::endl;
							}
						}, discernToken( nextToken, position ) );
						nextToken.clear();
					}
					// Don't ignore the token if it was an \n!
					if( next == '\n' ) {
						// Push the \n
						tokens.push( Token{ TokenType::Newline, position, {} } );
					}
					break;
				}
				default: {
					nextToken += next;
				}
			}

			if( next == '\n' ) {
				position.row++;
				position.column = 1;
			} else {
				position.column++;
			}
		}

		result = tokens;
		return result;
	}

	void printTokens( std::queue< Token > tokens ) {
		std::string output;

		while( !tokens.empty() ) {
			const Token& token = tokens.front();

			output += toString( token ) + "\n";

			tokens.pop();
		}

		std::cout << output << std::endl;
	}

}