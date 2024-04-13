static int value_to_extop(expr *vect, extop *eop, int32_t myseg)
{
    eop->type = EOT_DB_NUMBER;
    eop->val.num.offset = 0;
    eop->val.num.segment = eop->val.num.wrt = NO_SEG;
    eop->val.num.relative = false;

    for (; vect->type; vect++) {
        if (!vect->value)       /* zero term, safe to ignore */
            continue;

        if (vect->type <= EXPR_REG_END) /* false if a register is present */
            return -1;

        if (vect->type == EXPR_UNKNOWN) /* something we can't resolve yet */
            return 0;

        if (vect->type == EXPR_SIMPLE) {
            /* Simple number expression */
            eop->val.num.offset += vect->value;
            continue;
        }
        if (eop->val.num.wrt == NO_SEG && !eop->val.num.relative &&
            vect->type == EXPR_WRT) {
            /* WRT term */
            eop->val.num.wrt = vect->value;
            continue;
        }

        if (!eop->val.num.relative &&
            vect->type == EXPR_SEGBASE + myseg && vect->value == -1) {
            /* Expression of the form: foo - $ */
            eop->val.num.relative = true;
            continue;
        }

        if (eop->val.num.segment == NO_SEG &&
            vect->type >= EXPR_SEGBASE && vect->value == 1) {
            eop->val.num.segment = vect->type - EXPR_SEGBASE;
            continue;
        }

        /* Otherwise, badness */
        return -1;
    }

    /* We got to the end and it was all okay */
    return 0;
}