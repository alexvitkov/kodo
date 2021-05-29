#include <common.h>
#include <parser.h>
#include <error.h>
#include <ast.h>

// #define DEBUG_TOKENS

struct TokenStream {
    const std::vector<Token>* tokens;
    int position;

    void rewind() {
#ifdef DEBUG_TOKENS
        std::cout << "rewind()";
#endif
        position --;
        if (position < 0)
            position = 0;
    }

    Token pop() {
        if (position >= tokens->size())
            return {};

        Token t = (*tokens)[position++];
#ifdef DEBUG_TOKENS
        std::cout << "pop(" << t << ")\n";
#endif
        return t;
    }

    Token expect(Atom expected_atom) {
        if (position >= tokens->size()) {
            add_error(new UnexpectedTokenError(Token { ERR_ATOM_EOF }, expected_atom));
            return {};
        }

        Token t = (*tokens)[position++];
        if (t.atom != expected_atom) {
            add_error(new UnexpectedTokenError(t, expected_atom));
            return {};
        } else {
#ifdef DEBUG_TOKENS
        std::cout << "expect(" << t << ")\n";
#endif
            return t;
        }
    }

    Token peek() {
        if (position >= tokens->size())
            return {};
#ifdef DEBUG_TOKENS
        std::cout << "peek(" << (*tokens)[position] << ")\n";
#endif
        return (*tokens)[position];
    }
};




bool parse(Atom end);
AST_Function* parse_fn();
AST_Node* parse_expression();
AST_Block* parse_block();



static thread_local AST_Block* global;
thread_local AST_Block* current;
thread_local TokenStream ts;
thread_local AST_Block* block;




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
    MUST (ts.expect(KW_FN));

    AST_Function* fn = new AST_Function();

    Token t = ts.pop();
    if (t.atom.is_identifier()) {
    } else
        ts.rewind();

    MUST (ts.expect('('));

    while (true) {
        Token t = ts.pop();

        if (t.is_identifier()) {
            MUST (ts.expect(':'));

            AST_Node* expr = parse_expression();
            MUST (expr);

            MUST (fn->add_argument(t.atom, expr));
            t = ts.pop();
        }

        switch ((_atom_t)t) {
            case ',':
                break;
            case ')':
                goto DoneWithArguments;
        }
    }
DoneWithArguments:

    MUST (fn->body = parse_block());

    return fn;
}

AST_Node* parse_expression() {
    Token t = ts.peek();

    if (t.atom == KW_FN) {
        return parse_fn();
    }

    if (t.is_identifier())
        return new AST_Reference(t.atom);

    add_error(new UnexpectedTokenError(t, ERR_ATOM_ANY_IDENTIFIER));
    return nullptr;
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
