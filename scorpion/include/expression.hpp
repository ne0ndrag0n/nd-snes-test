#pragma once
#include "lex.hpp"
#include <variant>
#include <memory>

namespace Scorpion {

    using Expression = std::variant<
        struct AssignmentExpression,
        struct UnaryExpression,
        struct BinaryExpression,
        struct GroupExpression,
        struct Literal
    >;

    struct AssignmentExpression {
        Token symbol;
        std::unique_ptr< Expression > expression;
    };

    struct UnaryExpression {
        std::unique_ptr< Expression > expression;
        Token operatorToken;
    };

    struct BinaryExpression {
        std::unique_ptr< Expression > left;
        Token operatorToken;
        std::unique_ptr< Expression > right;
    };

    struct GroupExpression {
        std::unique_ptr< Expression > expression;
    };

    struct Literal {
        Token literalToken;
    };

    std::string toString( const Expression& expression );
}