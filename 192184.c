static int parse_mref(operand *op, const expr *e)
{
    int b, i, s;        /* basereg, indexreg, scale */
    int64_t o;          /* offset */

    b = op->basereg;
    i = op->indexreg;
    s = op->scale;
    o = op->offset;

    for (; e->type; e++) {
        if (e->type <= EXPR_REG_END) {
            bool is_gpr = is_class(REG_GPR,nasm_reg_flags[e->type]);

            if (is_gpr && e->value == 1 && b == -1) {
                /* It can be basereg */
                b = e->type;
            } else if (i == -1) {
                /* Must be index register */
                i = e->type;
                s = e->value;
            } else {
                if (b == -1)
                    nasm_nonfatal("invalid effective address: two index registers");
                else if (!is_gpr)
                    nasm_nonfatal("invalid effective address: impossible register");
                else
                    nasm_nonfatal("invalid effective address: too many registers");
                return -1;
            }
        } else if (e->type == EXPR_UNKNOWN) {
            op->opflags |= OPFLAG_UNKNOWN;
        } else if (e->type == EXPR_SIMPLE) {
            o += e->value;
        } else if  (e->type == EXPR_WRT) {
            op->wrt = e->value;
        } else if (e->type >= EXPR_SEGBASE) {
            if (e->value == 1) {
                if (op->segment != NO_SEG) {
                    nasm_nonfatal("invalid effective address: multiple base segments");
                    return -1;
                }
                op->segment = e->type - EXPR_SEGBASE;
            } else if (e->value == -1 &&
                       e->type == location.segment + EXPR_SEGBASE &&
                       !(op->opflags & OPFLAG_RELATIVE)) {
                op->opflags |= OPFLAG_RELATIVE;
            } else {
                nasm_nonfatal("invalid effective address: impossible segment base multiplier");
                return -1;
            }
        } else {
            nasm_nonfatal("invalid effective address: bad subexpression type");
            return -1;
        }
   }

    op->basereg  = b;
    op->indexreg = i;
    op->scale    = s;
    op->offset   = o;
    return 0;
}