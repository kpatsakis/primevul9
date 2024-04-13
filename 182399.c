zcvi(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    float fval;

    switch (r_type(op)) {
        case t_integer:
            return 0;
        case t_real:
            fval = op->value.realval;
            break;
        default:
            return_op_typecheck(op);
        case t_string:
            {
                ref str, token;
                int code;

                ref_assign(&str, op);
                code = gs_scan_string_token(i_ctx_p, &str, &token);
                if (code > 0)   /* anything other than a plain token */
                    code = gs_note_error(gs_error_syntaxerror);
                if (code < 0)
                    return code;
                switch (r_type(&token)) {
                    case t_integer:
                        *op = token;
                        return 0;
                    case t_real:
                        fval = token.value.realval;
                        break;
                    default:
                        return_error(gs_error_typecheck);
                }
            }
    }
    if (!REAL_CAN_BE_INT(fval))
        return_error(gs_error_rangecheck);
    make_int(op, (long)fval);   /* truncates towards 0 */
    return 0;
}