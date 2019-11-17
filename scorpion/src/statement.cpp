#include "statement.hpp"
#include "expression.hpp"
#include "utility.hpp"

namespace Scorpion {

	std::string toString( const Declaration& declaration ) {
		std::string result = "<unknown declaration>";

		std::visit( overloaded {
			[ &result ]( const ExpressionStatement& expressionStatement ) {
				result = "ExpressionStatement " + toString( expressionStatement );
			},
			[ &result ]( const DefineStatement& defineStatement ) {
				result = "DefineStatement ";

				result += "symbol: " + toString( defineStatement.symbol ) + " ";
				result += "type: " + toString( defineStatement.type ) + " ";

				if( defineStatement.expression ) {
					result += "expression: " + toString( *defineStatement.expression );
				} else {
					result += "<no valid expression>";
				}
			}
		}, declaration );

		return result;
	}

}