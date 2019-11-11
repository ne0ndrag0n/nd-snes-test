#pragma once

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace Scorpion {
	struct FilePosition {
		int row = 1;
		int column = 1;
	};

	constexpr unsigned int hash(const char *s, int off = 0) {
    	return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
	}
}