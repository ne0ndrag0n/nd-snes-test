#pragma once
#include "statement.hpp"
#include "token.hpp"
#include <optional>
#include <variant>
#include <stack>
#include <string>
#include <queue>

namespace Scorpion {

	std::variant< Program, std::stack< std::string > > compile( std::queue< Token >& tokens );

	void printTree( const Program& program );

}