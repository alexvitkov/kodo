#include <common.h>
#include <Type.h>
#include <Error.h>
#include <GlobalContext.h>
#include <Node/Scope.h>
#include <Node/Function.h>
#include <Node/Call.h>
#include <Node/NumberLiteral.h>
#include <Node/IfStatement.h>


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
IfStatement* parse_if();
Scope* parse_block();

// when hard_delimiter is hit, it is consumed and the expression is returned
// when soft_delimiter is hit, it is NOT consumed.
//
// for example parse_expression(',', ')') will stop at either a comma or closingn bracket
// but the closing brackets won't be consumed.
//
// rotate_tree is a helper argument that should always be true when calling a top-level parse_expression
Node* parse_expression(Atom hard_delimiter, Atom soft_delimiter, bool rotate_tree = true, bool in_brackets = false);



thread_local Scope* current_context;
thread_local TokenStream ts;
thread_local Scope* block;
thread_local bool bracket;





bool InputFile::parse() {
    current_context = global->scope;
    ts.tokens = &tokens;
    ts.position = 0;
    return ::parse(0); // we pass 0 (EOF) as delimiter, parse until end of file
}

// delimiter will be consumed
bool parse(Atom delimiter) {
    while (true) {
        Token top = ts.peek();
        if (top.atom == delimiter) {
            ts.pop();
            return true;
        }

        // parse expression, delimtiter is either ; or 'delimiter'
        // the ; will be consumed, 'delimiter' will not.
        Node* expr = parse_expression(';', delimiter);

        if (!expr) {
            // TODO make this not suck
            // it's a dirty dirty hack but to allow for empty expressions
            // we check the number of errors emited so far:
            // if there are no errors and nullptr is returned, it's an empty expression
            if (global->errors.size() == 0)
                continue;

            return false;
        }

        current_context->statements.push_back(expr);
    }
}

AST_Function* parse_fn() {
    // the fn keyword has been consumed before calling this function.
    AST_Function* fn = new AST_Function();

    Token tok = ts.peek();
    if (tok.is_identifier()) {
        ts.pop();
        fn->name = tok;
    }     

    // 
    if (ts.peek() == '[') {
        ts.pop();

        while (true) {
            Token id = ts.expect_id();

            if (ts.peek() == ':') {
                ts.pop();
                Node* type = parse_expression(',', ']'); // FIXME make sure this is a valid type
                MUST (type);

                fn->template_params.push_back(Parameter { id, (Type*)type });
            } else {
                fn->template_params.push_back(Parameter { id, nullptr });
            }

            if (ts.peek() == ',')
                ts.pop();
            else {
                MUST (ts.expect(']'));
                break;
            }
        }
    }

    MUST (ts.expect('('));

    // FIXME fn foo(x: int, ) is currently allowed
    while (ts.peek() != ')') {

        MUST (tok = ts.expect_id());
        MUST (ts.expect(':'));
        Node* expr = parse_expression(',', ')');
        MUST (expr);

        fn->params.push_back(Parameter { tok, (Type*)expr });

        fn->params.push_back({ tok, (Type*)expr });
    }

    ts.pop(); // discard the ')'

    if (ts.peek() == ':') {
        // parse return type
        ts.pop();
        Node* return_type = parse_expression(0, '{');
        MUST (return_type);
        fn->return_type = (Type*)return_type;
    }

    MUST (fn->body = parse_block());
    return fn;
}

IfStatement* parse_if() {
    // syntax for functions is "if ( EXPR ) THEN_BLOCK [else ELSE_BLOCK]
    // the if has already been popped

    IfStatement* ifs = new IfStatement(current_context);
    
    MUST (ts.expect('('))

    Scope* old_current = current_context;
    current_context = ifs->root_scope;

    MUST (ifs->condition = parse_expression(')', 0, true)); // this pops the closing bracket
    MUST (ifs->then_block = parse_block());

    if (ts.peek() == TOK_ELSE) {
        ts.pop();
        MUST (ifs->else_block = parse_block());
    }

    current_context = old_current;

    return ifs;
}

Node* parse_expression(Atom hard_delimiter, Atom soft_delimiter, bool rotate_tree, bool brackets) {
    Node* buildup = nullptr;

    while (true) {
        Token tok = ts.pop();

        if (tok.atom == TOK_NUMBER_LITERAL) {
            if (buildup) {
                add_error(new UnexpectedTokenError(tok, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            buildup = tok.nl;
            continue;
        }

        if (hard_delimiter && tok == hard_delimiter) {
            return buildup;
        }

        if (soft_delimiter && tok == soft_delimiter) {
            ts.rewind();
            return buildup;
        }

        if (tok == '{') {
            ts.rewind();
            buildup = parse_block();
            continue;
        }


        if (tok == '(') {
            if (buildup == nullptr) {
                buildup = parse_expression(')', 0);
                continue;
            } else {
                // Funciton call
                Call* new_call = new Call(buildup);

                while (ts.peek() != ')') {
                    Node* arg = parse_expression(',', ')', false, true);
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
            buildup = new UnresolvedRef(tok);
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
        } else if (tok == TOK_IF) {
            if (buildup) {
                add_error(new UnexpectedTokenError(tok, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            buildup = parse_if();
            if (!buildup)
                return nullptr;
            continue;
        }

        if (tok >= TOK_I8 && tok <= TOK_NUMBER_LITERAL) {
            if (buildup) {
                add_error(new UnexpectedTokenError(tok, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            switch (tok) {
                case TOK_NUMBER_LITERAL: 
                    buildup = &t_number_literal; 
                    break;
                case TOK_TYPE:
                    buildup = &t_type; 
                    break;
                default:
                    buildup = primitive_numeric_types[tok - TOK_I8];
                    break;
            }
            continue;
        }


        if (tok.is_infix_operator()) {
            if (!buildup) {
                add_error(new UnexpectedTokenError(tok, ERR_ATOM_ANY_EXPRESSION));
                return nullptr;
            }
            Node* rhs = parse_expression(hard_delimiter, soft_delimiter, false);
            if (!rhs)
                return nullptr;

            Call* call = new Call(tok, buildup, rhs);
            call->brackets = brackets;

            return rotate_tree ? call->rotate() : call;
        }

        add_error(new UnexpectedTokenError(tok, soft_delimiter ? soft_delimiter : hard_delimiter));
        return nullptr;
    }
}

Scope* parse_block() {
    Scope* block = new Scope(current_context);

    Scope* old_current = current_context;
    current_context = block;

    MUST (ts.expect('{'));

    MUST(parse('}'));

    current_context = old_current;
    return block;
}
