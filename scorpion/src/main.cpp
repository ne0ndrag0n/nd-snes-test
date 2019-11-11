#include "lex.hpp"
#include "utility.hpp"
#include <rang.hpp>
#include <iostream>

void printHelp() {
	std::cout << std::endl;
	std::cout
		<< rang::fg::green
		<< rang::style::bold
		<< "Options:"
		<< rang::style::reset
		<< std::endl;

	std::cout
		<< rang::fg::yellow
		<< rang::style::bold
		<< "-h"
		<< "\t"
		<< rang::style::reset
		<< "Show this help screen"
		<< std::endl;

	std::cout
		<< rang::fg::yellow
		<< rang::style::bold
		<< "-input (file)"
		<< "\t"
		<< rang::style::reset
		<< "Specify an input file"
		<< std::endl;

	std::cout
		<< rang::fg::yellow
		<< rang::style::bold
		<< "-dir (directory)"
		<< "\t"
		<< rang::style::reset
		<< "Specify an input directory"
		<< std::endl;

	std::cout
		<< rang::fg::yellow
		<< rang::style::bold
		<< "-link (file)"
		<< "\t"
		<< rang::style::reset
		<< "Specify a link script"
		<< std::endl;
}

int main( int argumentCount, char** arguments ) {
	std::cout
		<< rang::fg::cyan
		<< rang::style::bold
		<< "Scorpion Programming Language v0.0.1"
		<< rang::style::dim
		<< " (target: wdc65816)"
		<< rang::style::reset
		<< std::endl;

	std::cout
		<< "(c) 2019 Neon Dragon Research"
		<< std::endl;

	std::vector< std::string > paths;

	if( argumentCount == 1 ) {
		printHelp();
	} else {
		for( int i = 1; i < argumentCount; i++ ) {
			std::string argument( arguments[ i ] );
			if( argument == "-h" ) {
				printHelp();
			} else if( argument == "-input" ) {
				// FIXME don't do this
				i++;
				paths.emplace_back( arguments[ i ] );
			} else {
				std::cout << rang::fg::red << rang::style::bold << "error: " << rang::style::reset << " invalid argument: " << argument << std::endl;
			}
		}
	}

	if( paths.size() ) {
		for( const std::string& path : paths ) {
			auto result = Scorpion::getTokens( path );
			std::visit( overloaded {
				[]( std::queue< Scorpion::Token > tokens ) {
					Scorpion::printTokens( tokens );
				},
				[]( std::string error ) {
					std::cout << error << std::endl;
				}
			}, result );
		}
	}

	return 0;
}