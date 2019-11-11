#pragma once
#include <optional>
#include <string>
#include <variant>

namespace Scorpion {

	enum class TokenType {
		Define,
		Symbol,
		As,
		Dot,
		Plus,
		Minus,
		Asterisk,
		ForwardSlash,
		BackwardSlash,
		Equals,
		LeftParen,
		RightParen,
		NumericLiteral,
		DecimalLiteral,
		StringLiteral,
		TypeU8,
		TypeU16,
		TypeU32,
		Newline
	};

	struct Token {
		TokenType type;
		std::optional< std::variant< std::string, long, double > > value;
	};

}