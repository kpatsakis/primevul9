convert_to_string(const gs_memory_t *mem, os_ptr op1, os_ptr op)
{
    uint len;
    const byte *pstr = 0;
    int code = obj_cvs(mem, op1, op->value.bytes, r_size(op), &len, &pstr);

    if (code < 0) {
        /*
         * Some common downloaded error handlers assume that
         * operator names don't exceed a certain fixed size.
         * To work around this bit of bad design, we implement
         * a special hack here: if we got a rangecheck, and
         * the object is an operator whose name begins with
         * %, ., or @, we just truncate the name.
         */
        if (code == gs_error_rangecheck)
            switch (r_btype(op1)) {
                case t_oparray:
                case t_operator:
                    if (pstr != 0)
                        switch (*pstr) {
                            case '%':
                            case '.':
                            case '@':
                                len = r_size(op);
                                memcpy(op->value.bytes, pstr, len);
                                goto ok;
                        }
            }
        return code;
    }
ok:
    *op1 = *op;
    r_set_size(op1, len);
    return 0;
}