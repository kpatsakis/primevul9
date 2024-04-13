print_active_line(const gs_memory_t *mem, const char *label, const active_line * alp)
{
    dmlprintf5(mem, "[f]%s 0x%lx(%d): x_current=%f x_next=%f\n",
               label, (ulong) alp, alp->direction,
               fixed2float(alp->x_current), fixed2float(alp->x_next));
    dmlprintf5(mem, "    start=(%f,%f) pt_end=0x%lx(%f,%f)\n",
               fixed2float(alp->start.x), fixed2float(alp->start.y),
               (ulong) alp->pseg,
               fixed2float(alp->end.x), fixed2float(alp->end.y));
    dmlprintf2(mem, "    prev=0x%lx next=0x%lx\n",
               (ulong) alp->prev, (ulong) alp->next);
}