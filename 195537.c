get_operator(struct compiling *c, const node *n)
{
    switch (TYPE(n)) {
        case VBAR:
            return BitOr;
        case CIRCUMFLEX:
            return BitXor;
        case AMPER:
            return BitAnd;
        case LEFTSHIFT:
            return LShift;
        case RIGHTSHIFT:
            return RShift;
        case PLUS:
            return Add;
        case MINUS:
            return Sub;
        case STAR:
            return Mult;
        case AT:
            if (c->c_feature_version < 5) {
                ast_error(c, n,
                          "The '@' operator is only supported in Python 3.5 and greater");
                return (operator_ty)0;
            }
            return MatMult;
        case SLASH:
            return Div;
        case DOUBLESLASH:
            return FloorDiv;
        case PERCENT:
            return Mod;
        default:
            return (operator_ty)0;
    }
}