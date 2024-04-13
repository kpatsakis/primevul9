zcvr(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    switch (r_type(op)) {
        case t_integer:
            make_real(op, (float)op->value.intval);
        case t_real:
            return 0;
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
                        make_real(op, (float)token.value.intval);
                        return 0;
                    case t_real:
                        *op = token;
                        return 0;
                    default:
                        return_error(gs_error_typecheck);
                }
            }
    }
}