#pragma once
#include "utility.hpp"
#include <optional>
#include <string>
#include <variant>

namespace Scorpion {

	enum class TokenType {
		None,
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
		False,
		True,
		Null,
		And,
		Or,
		Not,
		GreaterThan,
		GreaterThanEqual,
		LessThan,
		LessThanEqual,
		EqualTo,
		NotEqualTo,
		Newline
	};

	struct Token {
		TokenType type;
		FilePosition position;
		std::optional< std::variant< std::string, long, double > > value;
	};

}