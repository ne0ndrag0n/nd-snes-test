#pragma once
#include <string>
#include <variant>

namespace Scorpion {
	using Literal = std::variant< std::string, long, double >;
}