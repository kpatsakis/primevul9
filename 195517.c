ast_for_augassign(struct compiling *c, const node *n)
{
    REQ(n, augassign);
    n = CHILD(n, 0);
    switch (STR(n)[0]) {
        case '+':
            return Add;
        case '-':
            return Sub;
        case '/':
            if (STR(n)[1] == '/')
                return FloorDiv;
            else
                return Div;
        case '%':
            return Mod;
        case '<':
            return LShift;
        case '>':
            return RShift;
        case '&':
            return BitAnd;
        case '^':
            return BitXor;
        case '|':
            return BitOr;
        case '*':
            if (STR(n)[1] == '*')
                return Pow;
            else
                return Mult;
        case '@':
            if (c->c_feature_version < 5) {
                ast_error(c, n,
                          "The '@' operator is only supported in Python 3.5 and greater");
                return (operator_ty)0;
            }
            return MatMult;
        default:
            PyErr_Format(PyExc_SystemError, "invalid augassign: %s", STR(n));
            return (operator_ty)0;
    }
}