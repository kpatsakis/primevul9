static bool parse_braces(decoflags_t *decoflags)
{
    int i, j;

    i = tokval.t_type;

    while (true) {
        switch (i) {
        case TOKEN_OPMASK:
            if (*decoflags & OPMASK_MASK) {
                nasm_nonfatal("opmask k%"PRIu64" is already set",
                              *decoflags & OPMASK_MASK);
                *decoflags &= ~OPMASK_MASK;
            }
            *decoflags |= VAL_OPMASK(nasm_regvals[tokval.t_integer]);
            break;
        case TOKEN_DECORATOR:
            j = tokval.t_integer;
            switch (j) {
            case BRC_Z:
                *decoflags |= Z_MASK;
                break;
            case BRC_1TO2:
            case BRC_1TO4:
            case BRC_1TO8:
            case BRC_1TO16:
                *decoflags |= BRDCAST_MASK | VAL_BRNUM(j - BRC_1TO2);
                break;
            default:
                nasm_nonfatal("{%s} is not an expected decorator",
                              tokval.t_charptr);
                break;
            }
            break;
        case ',':
        case TOKEN_EOS:
            return false;
        default:
            nasm_nonfatal("only a series of valid decorators expected");
            return true;
        }
        i = stdscan(NULL, &tokval);
    }
}