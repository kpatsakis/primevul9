static void mref_set_optype(operand *op)
{
    int b = op->basereg;
    int i = op->indexreg;
    int s = op->scale;

    /* It is memory, but it can match any r/m operand */
    op->type |= MEMORY_ANY;

    if (b == -1 && (i == -1 || s == 0)) {
        int is_rel = globalbits == 64 &&
            !(op->eaflags & EAF_ABS) &&
            ((globalrel &&
              !(op->eaflags & EAF_FSGS)) ||
             (op->eaflags & EAF_REL));

        op->type |= is_rel ? IP_REL : MEM_OFFS;
    }

    if (i != -1) {
        opflags_t iclass = nasm_reg_flags[i];

        if (is_class(XMMREG,iclass))
            op->type |= XMEM;
        else if (is_class(YMMREG,iclass))
            op->type |= YMEM;
        else if (is_class(ZMMREG,iclass))
            op->type |= ZMEM;
    }
}