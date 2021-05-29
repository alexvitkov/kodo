#include <common.h>
#include <parser.h>
#include <error.h>
#include <ast.h>

//#define DEBUG_TOKENS

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
AST_Node* parse_expression(Atom hard_delimiter, Atom soft_delimiter);
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

        AST_Node* expr = parse_expression(';', '}');
        if (!expr) 
            return false;

        current->statements.push_back(expr);
    }
}

AST_Function* parse_fn() {
    AST_Function* fn = new AST_Function();

    Token tok = ts.peek();
    if (tok.is_identifier()) {
        ts.pop();

        current->define(tok, fn);
    }     

    MUST (ts.expect('('));

    while (true) {
        // FIXME MAYBE fn foo(x: int, ) is currently allowed
        tok = ts.pop();
        MUST (tok);

        if (tok.is_identifier()) {
            MUST (ts.expect(':'));

            AST_Node* expr = parse_expression(',', ')');
            MUST (expr);

            MUST (fn->add_argument(tok.atom, expr));
            tok = ts.pop();

            if (tok.atom == ')')
                break;
        } else
            break;
    }

    MUST (fn->body = parse_block());
    return fn;
}

AST_Node* parse_expression(Atom hard_delimiter, Atom soft_delimiter) {
    AST_Node* buildup = nullptr;

    while (true) {
        Token tok = ts.pop();

        if (hard_delimiter && tok == hard_delimiter)
            return buildup;

        if (soft_delimiter && tok == soft_delimiter) {
            ts.rewind();
            return buildup;
        }

        if (tok == '(') {
            if (buildup == nullptr) {
                buildup = parse_expression(')', 0);
                continue;
            } else {
                // Funciton call
                AST_Call* new_call = new AST_Call(buildup);

                while (ts.peek() != ')') {
                    AST_Node* arg = parse_expression(',', ')');
                    MUST (arg);
                    new_call->args.push_back(arg);
                }

                ts.pop(); // pop the closing bracket
                buildup = new_call;
                continue;
            }
        }


        if (tok.is_identifier()) {
            if (buildup) {
                add_error(new UnexpectedTokenError(tok, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            buildup = new AST_Reference(tok);
            continue;
        }

        if (tok == TOK_FN) {
            if (buildup) {
                add_error(new UnexpectedTokenError(tok, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            buildup = parse_fn();
            if (!buildup)
                return nullptr;
            continue;
        }

        if (tok.is_infix_operator()) {
            if (!buildup) {
                add_error(new UnexpectedTokenError(tok, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            AST_Node* rhs = parse_expression(hard_delimiter, soft_delimiter);
            if (!rhs)
                return nullptr;

            // without precedence checks, a * b + c would parse down to this without respecting precedence:
            //      *
            //     / \
            //    a   +
            //       / \
            //      b   c
            // to fix this we rotate the tree in some cases.
            // if the right-hand side operator (rhs_cll->fn) has lower precedence than the top operator (tok),
            // or the operator is left associative and the precedences are equal, we rotate the tree:
            //      +
            //     / \
            //    *   c       
            //   / \
            //  a   b      
            AST_Call* rhs_call = dynamic_cast<AST_Call*>(rhs);
            if (rhs_call) {
                int rhs_call_prec = 10000;
                auto _fn = dynamic_cast<AST_Reference*>(rhs_call->fn);

                if (_fn && _fn->atom.precedence() < (tok.precedence() + tok.associativity()) && !rhs_call->brackets) {
                    // construct rotated tree, the rhs node is now on top
                    AST_Call* call = new AST_Call(tok, buildup, rhs_call->args[0]);
                    call->brackets = hard_delimiter == ')';
                    rhs_call->args[0] = call;
                    return rhs_call;
                }
            }

            AST_Call* call = new AST_Call(tok, buildup, rhs);
            call->brackets = hard_delimiter == ')';
            return call;
        }

        add_error(new UnexpectedTokenError(tok, soft_delimiter ? soft_delimiter : hard_delimiter));
        return nullptr;
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
