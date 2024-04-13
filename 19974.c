static MemTxResult flatview_read(struct uc_struct *uc, FlatView *fv, hwaddr addr,
                                 MemTxAttrs attrs, void *buf, hwaddr len)
{
    hwaddr l;
    hwaddr addr1;
    MemoryRegion *mr;

    l = len;
    mr = flatview_translate(uc, fv, addr, &addr1, &l, false, attrs);
    return flatview_read_continue(uc, fv, addr, attrs, buf, len,
                                  addr1, l, mr);
}