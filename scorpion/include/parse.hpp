#pragma once
#include "statement.hpp"
#include "token.hpp"
#include <optional>
#include <queue>

namespace Scorpion {

	std::optional< Program > getProgram( std::queue< Token >& tokens );

	void printTree( const Program& program );

}