print_line_list(const gs_memory_t *mem, const active_line * flp)
{
    const active_line *lp;

    for (lp = flp; lp != 0; lp = lp->next) {
        fixed xc = lp->x_current, xn = lp->x_next;

        dmlprintf3(mem, "[f]0x%lx(%d): x_current/next=%g",
                  (ulong) lp, lp->direction,
                  fixed2float(xc));
        if (xn != xc)
            dmprintf1(mem, "/%g", fixed2float(xn));
        dmputc(mem, '\n');
    }
}