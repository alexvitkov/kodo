#include <common.h>
#include <parser.h>
#include <error.h>
#include <ast.h>

#define DEBUG_TOKENS

struct TokenStream {
    const std::vector<Token>* tokens;
    int position;
void rewind() {
#ifdef DEBUG_TOKENS
        std::cout << "rewind()\n";
#endif
        position --;
        if (position < 0)
            position = 0;
    }

    Token pop() {
        position++;
        if (position - 1 >= tokens->size()) {
            int add = position - tokens->size() - 1;
#ifdef DEBUG_TOKENS
            std::cout << "pop() -> EOF";
            if (add) std::cout << " + " << add;
            std::cout << "\n";
#endif
            return TOK_NONE;
        }

        Token t = (*tokens)[position - 1];
#ifdef DEBUG_TOKENS
        std::cout << "pop() -> " << t << "\n";
#endif
        return t;
    }

    Token expect(Atom expected_atom) {
        if (position >= tokens->size()) {
#ifdef DEBUG_TOKENS
            std::cout << "expect(" << expected_atom << ") -> EOF\n";
#endif
            add_error(new UnexpectedTokenError(Token { ERR_ATOM_EOF }, expected_atom));
            return 0;
        }

        Token t = (*tokens)[position++];
#ifdef DEBUG_TOKENS
        std::cout << "expect(" << expected_atom << ") -> " << t << "\n";
#endif
        if (t.atom != expected_atom) {
            add_error(new UnexpectedTokenError(t, expected_atom));
            return 0;
        } else {
            return t;
        }

    }

    Token expect_id() {
        if (position >= tokens->size()) {
            add_error(new UnexpectedTokenError(Token { ERR_ATOM_EOF }, ERR_ATOM_ANY_IDENTIFIER));
            return 0;
        }

        Token t = (*tokens)[position++];
#ifdef DEBUG_TOKENS
        std::cout << "expect_id() -> " << t << "\n";
#endif
        if (!t.is_identifier()) {
            add_error(new UnexpectedTokenError(t, ERR_ATOM_ANY_IDENTIFIER));
            return 0;
        } else {
            return t;
        }
    }

    Token peek() {
        if (position >= tokens->size())
            return 0;
#ifdef DEBUG_TOKENS
        std::cout << "peek() -> " << (*tokens)[position] << "\n";
#endif
        return (*tokens)[position];
    }
};




bool parse(Atom end);
AST_Function* parse_fn();
AST_Node* parse_expression(Atom hard_delimiter = 0);
AST_Block* parse_block();



static thread_local AST_Block* global;
thread_local AST_Block* current;
thread_local TokenStream ts;
thread_local AST_Block* block;
thread_local bool bracket;




bool parse(AST_Block* _global, const std::vector<Token>& tokens) {
    global = _global;
    current = global;
    ts.tokens = &tokens;
    return parse(0);
}

bool parse(Atom end) {
    while (true) {
        Token top = ts.peek();
        if (top.atom == end) {
            ts.pop();
            return true;
        }

        AST_Node* expr = parse_expression();
        if (!expr) 
            return false;

        current->statements.push_back(expr);
    }
}

AST_Function* parse_fn() {
    MUST (ts.expect(TOK_FN));

    AST_Function* fn = new AST_Function();

    Token t = ts.pop();
    if (t.is_identifier()) {
    } else
        ts.rewind();

    MUST (ts.expect('('));

    while (true) {
        // FIXME MAYBE fn foo(x: int, ) is currently allowed
        t = ts.pop();
        MUST (t);

        if (t.is_identifier()) {
            MUST (ts.expect(':'));

            AST_Node* expr = parse_expression();
            MUST (expr);

            MUST (fn->add_argument(t.atom, expr));
            t = ts.pop();

            if (t.atom != ',')
                break;
        } else
            break;
    }

    if (t.atom != ')') {
        add_error(new UnexpectedTokenError(t, ')'));
        return nullptr;
    }

    MUST (fn->body = parse_block());
    return fn;
}

AST_Node* parse_expression(Atom hard_delimiter) {

    AST_Node* buildup = nullptr;

    std::cout << "parse_expression(" << hard_delimiter << ")\n";

    while (true) {
        Token t = ts.pop();

        if (hard_delimiter && t == hard_delimiter) {
            std::cout << "parse_expression() returning via delim\n";
            return buildup;
        }

        if (t == '(') {
            if (buildup == nullptr) {
                buildup = parse_expression(')');
                continue;
            }
            else {
                NOT_IMPLEMENTED(); // Function call
            }
        }

        if (t.is_identifier()) {
            if (buildup) {
                add_error(new UnexpectedTokenError(t, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            buildup = new AST_Reference(t);
            continue;
        } 

        if (t.is_infix_operator()) {
            if (!buildup) {
                add_error(new UnexpectedTokenError(t, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            AST_Node* rhs = parse_expression();
            if (!rhs)
                return nullptr;
            buildup = new AST_Call(t, buildup, rhs);
            continue;
        }

        // we've hit a token that isn't part of this expression
        if (!hard_delimiter) {
            // if there's no delimiter, pretend we've never seen this and be done
            ts.rewind();
            std::cout << "parse_expression() returning normally\n";
            return buildup;
        } else {
            // if there's an explicit delimiter, this is an error.
            add_error(new UnexpectedTokenError(t, hard_delimiter));
            return nullptr;
        }


    }


}

AST_Block* parse_block() {
    AST_Block* block = new AST_Block();

    AST_Block* old_current = current;
    current = block;

    MUST (ts.expect('{'));

    MUST(parse('}'));

    current = old_current;
    return block;
}
