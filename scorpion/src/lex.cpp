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

			switch( token.type ) {
				case TokenType::Define:
					output += "TokenType::Define";
					break;
				case TokenType::Symbol:
					output += "TokenType::Symbol(" + std::get< std::string >( *token.value ) + ")";
					break;
				case TokenType::As:
					output += "TokenType::As";
					break;
				case TokenType::Dot:
					output += "TokenType::Dot";
					break;
				case TokenType::Plus:
					output += "TokenType::Plus";
					break;
				case TokenType::Minus:
					output += "TokenType::Minus";
					break;
				case TokenType::Asterisk:
					output += "TokenType::Asterisk";
					break;
				case TokenType::ForwardSlash:
					output += "TokenType::ForwardSlash";
					break;
				case TokenType::BackwardSlash:
					output += "TokenType::BackwardSlash";
					break;
				case TokenType::Equals:
					output += "TokenType::Equals";
					break;
				case TokenType::LeftParen:
					output += "TokenType::LeftParen";
					break;
				case TokenType::RightParen:
					output += "TokenType::RightParen";
					break;
				case TokenType::NumericLiteral:
					output += "TokenType::NumericLiteral(" + std::to_string( std::get< long >( *token.value ) ) + ")";
					break;
				case TokenType::DecimalLiteral:
					output += "TokenType::DecimalLiteral(" + std::to_string( std::get< double >( *token.value ) ) + ")";
					break;
				case TokenType::StringLiteral:
					output += "TokenType::StringLiteral(" + std::get< std::string >( *token.value ) + ")";
					break;
				case TokenType::TypeU8:
					output += "TokenType::TypeU8";
					break;
				case TokenType::TypeU16:
					output += "TokenType::TypeU16";
					break;
				case TokenType::TypeU32:
					output += "TokenType::TypeU32";
					break;
				case TokenType::Newline:
					output += "TokenType::Newline";
					break;
				default:
					output += "Invalid/Unknown token";
					break;
			}

			output += "\n";

			tokens.pop();
		}

		std::cout << output << std::endl;
	}

}