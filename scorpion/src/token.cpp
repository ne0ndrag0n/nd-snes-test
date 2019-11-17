#include "token.hpp"
#include "utility.hpp"

namespace Scorpion {

	const char* getTypeString( const Token& token ) {
		switch( token.type ) {
			case TokenType::None:
				return "TokenType::None";
			case TokenType::Define:
				return "TokenType::Define";
			case TokenType::Symbol:
				return "TokenType::Symbol";
			case TokenType::As:
				return "TokenType::As";
			case TokenType::Dot:
				return "TokenType::Dot";
			case TokenType::Plus:
				return "TokenType::Plus";
			case TokenType::Minus:
				return "TokenType::Minus";
			case TokenType::Asterisk:
				return "TokenType::Asterisk";
			case TokenType::ForwardSlash:
				return "TokenType::ForwardSlash";
			case TokenType::BackwardSlash:
				return "TokenType::BackwardSlash";
			case TokenType::Equals:
				return "TokenType::Equals";
			case TokenType::LeftParen:
				return "TokenType::LeftParen";
			case TokenType::RightParen:
				return "TokenType::RightParen";
			case TokenType::NumericLiteral:
				return "TokenType::NumericLiteral";
			case TokenType::DecimalLiteral:
				return "TokenType::DecimalLiteral";
			case TokenType::StringLiteral:
				return "TokenType::StringLiteral";
			case TokenType::TypeU8:
				return "TokenType::TypeU8";
			case TokenType::TypeU16:
				return "TokenType::TypeU16";
			case TokenType::TypeU32:
				return "TokenType::TypeU32";
			case TokenType::TypeS8:
				return "TokenType::TypeS8";
			case TokenType::TypeS16:
				return "TokenType::TypeS16";
			case TokenType::TypeS32:
				return "TokenType::TypeS32";
			case TokenType::TypeReal:
				return "TokenType::TypeReal";
			case TokenType::TypeString:
				return "TokenType::TypeString";
			case TokenType::TypeBoolean:
				return "TokenType::TypeBoolean";
			case TokenType::False:
				return "TokenType::False";
			case TokenType::True:
				return "TokenType::True";
			case TokenType::Null:
				return "TokenType::Null";
			case TokenType::And:
				return "TokenType::And";
			case TokenType::Or:
				return "TokenType::Or";
			case TokenType::Not:
				return "TokenType::Not";
			case TokenType::GreaterThan:
				return "TokenType::GreaterThan";
			case TokenType::GreaterThanEqual:
				return "TokenType::GreaterThanEqual";
			case TokenType::LessThan:
				return "TokenType::LessThan";
			case TokenType::LessThanEqual:
				return "TokenType::LessThanEqual";
			case TokenType::EqualTo:
				return "TokenType::EqualTo";
			case TokenType::NotEqualTo:
				return "TokenType::NotEqualTo";
			case TokenType::Newline:
				return "TokenType::Newline";
			default:
				return "<unknown token>";
		}
	}

	std::string toString( const Token& token ) {
		std::string output;

		switch( token.type ) {
			case TokenType::Symbol:
				output += "TokenType::Symbol(" + std::get< std::string >( *token.value ) + ")";
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
			default:
				output += getTypeString( token );
		}

		return output;
	}

}