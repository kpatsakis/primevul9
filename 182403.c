zcvrs(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int radix;

    check_type(op[-1], t_integer);
    if (op[-1].value.intval < 2 || op[-1].value.intval > 36)
        return_error(gs_error_rangecheck);
    radix = op[-1].value.intval;
    check_write_type(*op, t_string);
    if (radix == 10) {
        switch (r_type(op - 2)) {
            case t_integer:
            case t_real:
                {
                    int code = convert_to_string(imemory, op - 2, op);

                    if (code < 0)
                        return code;
                    pop(2);
                    return 0;
                }
            case t__invalid:
                return_error(gs_error_stackunderflow);
            default:
                return_error(gs_error_rangecheck); /* CET 24-05 wants rangecheck */
        }
    } else {
        uint ival;
        byte digits[sizeof(ulong) * 8];
        byte *endp = &digits[countof(digits)];
        byte *dp = endp;

        switch (r_type(op - 2)) {
            case t_integer:
                ival = (uint) op[-2].value.intval;
                break;
            case t_real:
                {
                    float fval = op[-2].value.realval;

                    if (!REAL_CAN_BE_INT(fval))
                        return_error(gs_error_rangecheck);
                    ival = (ulong) (long)fval;
                } break;
            case t__invalid:
                return_error(gs_error_stackunderflow);
            default:
                return_error(gs_error_rangecheck); /* CET 24-05 wants rangecheck */
        }
        do {
            int dit = ival % radix;

            *--dp = dit + (dit < 10 ? '0' : ('A' - 10));
            ival /= radix;
        }
        while (ival);
        if (endp - dp > r_size(op))
            return_error(gs_error_rangecheck);
        memcpy(op->value.bytes, dp, (uint) (endp - dp));
        r_set_size(op, endp - dp);
    }
    op[-2] = *op;
    pop(2);
    return 0;
}