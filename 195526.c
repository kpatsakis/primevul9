ast_for_expr(struct compiling *c, const node *n)
{
    /* handle the full range of simple expressions
       namedexpr_test: test [':=' test]
       test: or_test ['if' or_test 'else' test] | lambdef
       test_nocond: or_test | lambdef_nocond
       or_test: and_test ('or' and_test)*
       and_test: not_test ('and' not_test)*
       not_test: 'not' not_test | comparison
       comparison: expr (comp_op expr)*
       expr: xor_expr ('|' xor_expr)*
       xor_expr: and_expr ('^' and_expr)*
       and_expr: shift_expr ('&' shift_expr)*
       shift_expr: arith_expr (('<<'|'>>') arith_expr)*
       arith_expr: term (('+'|'-') term)*
       term: factor (('*'|'@'|'/'|'%'|'//') factor)*
       factor: ('+'|'-'|'~') factor | power
       power: atom_expr ['**' factor]
       atom_expr: [AWAIT] atom trailer*
       yield_expr: 'yield' [yield_arg]
    */

    asdl_seq *seq;
    int i;

 loop:
    switch (TYPE(n)) {
        case namedexpr_test:
            if (NCH(n) == 3)
                return ast_for_namedexpr(c, n);
            /* Fallthrough */
        case test:
        case test_nocond:
            if (TYPE(CHILD(n, 0)) == lambdef ||
                TYPE(CHILD(n, 0)) == lambdef_nocond)
                return ast_for_lambdef(c, CHILD(n, 0));
            else if (NCH(n) > 1)
                return ast_for_ifexpr(c, n);
            /* Fallthrough */
        case or_test:
        case and_test:
            if (NCH(n) == 1) {
                n = CHILD(n, 0);
                goto loop;
            }
            seq = _Py_asdl_seq_new((NCH(n) + 1) / 2, c->c_arena);
            if (!seq)
                return NULL;
            for (i = 0; i < NCH(n); i += 2) {
                expr_ty e = ast_for_expr(c, CHILD(n, i));
                if (!e)
                    return NULL;
                asdl_seq_SET(seq, i / 2, e);
            }
            if (!strcmp(STR(CHILD(n, 1)), "and"))
                return BoolOp(And, seq, LINENO(n), n->n_col_offset,
                              n->n_end_lineno, n->n_end_col_offset,
                              c->c_arena);
            assert(!strcmp(STR(CHILD(n, 1)), "or"));
            return BoolOp(Or, seq, LINENO(n), n->n_col_offset,
                          n->n_end_lineno, n->n_end_col_offset, c->c_arena);
        case not_test:
            if (NCH(n) == 1) {
                n = CHILD(n, 0);
                goto loop;
            }
            else {
                expr_ty expression = ast_for_expr(c, CHILD(n, 1));
                if (!expression)
                    return NULL;

                return UnaryOp(Not, expression, LINENO(n), n->n_col_offset,
                               n->n_end_lineno, n->n_end_col_offset,
                               c->c_arena);
            }
        case comparison:
            if (NCH(n) == 1) {
                n = CHILD(n, 0);
                goto loop;
            }
            else {
                expr_ty expression;
                asdl_int_seq *ops;
                asdl_seq *cmps;
                ops = _Py_asdl_int_seq_new(NCH(n) / 2, c->c_arena);
                if (!ops)
                    return NULL;
                cmps = _Py_asdl_seq_new(NCH(n) / 2, c->c_arena);
                if (!cmps) {
                    return NULL;
                }
                for (i = 1; i < NCH(n); i += 2) {
                    cmpop_ty newoperator;

                    newoperator = ast_for_comp_op(c, CHILD(n, i));
                    if (!newoperator) {
                        return NULL;
                    }

                    expression = ast_for_expr(c, CHILD(n, i + 1));
                    if (!expression) {
                        return NULL;
                    }

                    asdl_seq_SET(ops, i / 2, newoperator);
                    asdl_seq_SET(cmps, i / 2, expression);
                }
                expression = ast_for_expr(c, CHILD(n, 0));
                if (!expression) {
                    return NULL;
                }

                return Compare(expression, ops, cmps, LINENO(n), n->n_col_offset,
                               n->n_end_lineno, n->n_end_col_offset, c->c_arena);
            }
            break;

        case star_expr:
            return ast_for_starred(c, n);
        /* The next five cases all handle BinOps.  The main body of code
           is the same in each case, but the switch turned inside out to
           reuse the code for each type of operator.
         */
        case expr:
        case xor_expr:
        case and_expr:
        case shift_expr:
        case arith_expr:
        case term:
            if (NCH(n) == 1) {
                n = CHILD(n, 0);
                goto loop;
            }
            return ast_for_binop(c, n);
        case yield_expr: {
            node *an = NULL;
            node *en = NULL;
            int is_from = 0;
            expr_ty exp = NULL;
            if (NCH(n) > 1)
                an = CHILD(n, 1); /* yield_arg */
            if (an) {
                en = CHILD(an, NCH(an) - 1);
                if (NCH(an) == 2) {
                    is_from = 1;
                    exp = ast_for_expr(c, en);
                }
                else
                    exp = ast_for_testlist(c, en);
                if (!exp)
                    return NULL;
            }
            if (is_from)
                return YieldFrom(exp, LINENO(n), n->n_col_offset,
                                 n->n_end_lineno, n->n_end_col_offset, c->c_arena);
            return Yield(exp, LINENO(n), n->n_col_offset,
                         n->n_end_lineno, n->n_end_col_offset, c->c_arena);
        }
        case factor:
            if (NCH(n) == 1) {
                n = CHILD(n, 0);
                goto loop;
            }
            return ast_for_factor(c, n);
        case power:
            return ast_for_power(c, n);
        default:
            PyErr_Format(PyExc_SystemError, "unhandled expr: %d", TYPE(n));
            return NULL;
    }
    /* should never get here unless if error is set */
    return NULL;
}