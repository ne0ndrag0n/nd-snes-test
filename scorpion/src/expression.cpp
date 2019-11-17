#include "expression.hpp"
#include "utility.hpp"
#include "token.hpp"

namespace Scorpion {

	const char* getTypeString( const Expression& expression ) {
		const char* result = "<unknown expression>";

		std::visit( overloaded {
			[ &result ]( const AssignmentExpression& ) {
				result = "AssignmentExpression";
			},
			[ &result ]( const UnaryExpression& ) {
				result = "UnaryExpression";
			},
			[ &result ]( const BinaryExpression& ) {
				result = "BinaryExpression";
			},
			[ &result ]( const GroupExpression& ) {
				result = "GroupExpression";
			},
			[ &result ]( const Literal& ) {
				result = "Literal";
			}
		}, expression );

		return result;
	}

	std::string toString( const Expression& expression ) {
		std::string result = getTypeString( expression );
		result += " ";

		std::visit( overloaded {
			[ &result ]( const AssignmentExpression& expression ) {
				result += toString( expression.symbol );
				result += ": ";
				if( expression.expression ) {
					result += toString( *expression.expression );
				} else {
					result += "<no assignment expression>";
				}
			},
			[ &result ]( const UnaryExpression& expression ) {
				result += toString( expression.operatorToken );
				result += ": ";
				if( expression.expression ) {
					result += toString( *expression.expression );
				} else {
					result += "<no unary expression>";
				}
			},
			[ &result ]( const BinaryExpression& expression ) {
				result += "lhs: ";
				if( expression.left ) {
					result += toString( *expression.left );
				} else {
					result += "<no lhs expression>";
				}

				result += toString( expression.operatorToken ) + " ";
				result += "rhs: ";

				if( expression.right ) {
					result += toString( *expression.right );
				} else {
					result += "<no rhs expression>";
				}
			},
			[ &result ]( const GroupExpression& expression ) {
				if( expression.expression ) {
					result += toString( *expression.expression );
				} else {
					result += "<no group expression>";
				}
			},
			[ &result ]( const Literal& expression ) {
				result += toString( expression.literalToken );
			}
		}, expression );

		return result;
	}

}