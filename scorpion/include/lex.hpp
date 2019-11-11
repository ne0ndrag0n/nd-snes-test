#pragma once
#include "token.hpp"
#include <variant>
#include <queue>
#include <string>

namespace Scorpion {

	std::variant< std::queue< Token >, std::string > getTokens( const std::string& filename );
	void printTokens( std::queue< Token > tokens );

}