static MemTxResult subpage_read(struct uc_struct *uc, void *opaque, hwaddr addr, uint64_t *data,
                                unsigned len, MemTxAttrs attrs)
{
    subpage_t *subpage = opaque;
    uint8_t buf[8];
    MemTxResult res;

#if defined(DEBUG_SUBPAGE)
    printf("%s: subpage %p len %u addr " TARGET_FMT_plx "\n", __func__,
           subpage, len, addr);
#endif
    res = flatview_read(uc, subpage->fv, addr + subpage->base, attrs, buf, len);
    if (res) {
        return res;
    }
    *data = ldn_p(buf, len);
    return MEMTX_OK;
}