#pragma once
#include "expression.hpp"
#include <vector>
#include <variant>

namespace Scorpion {

	using ExpressionStatement = Expression;

	struct DefineStatement {
		Token symbol;
		Token type;
		std::unique_ptr< Expression > expression;
	};

	using Program = std::vector< std::variant< ExpressionStatement, DefineStatement > >;

}