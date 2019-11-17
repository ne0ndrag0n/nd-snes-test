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

	using Declaration = std::variant< ExpressionStatement, DefineStatement >;
	using Program = std::vector< Declaration >;

}