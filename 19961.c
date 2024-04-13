static bool subpage_accepts(struct uc_struct *uc, void *opaque, hwaddr addr,
                            unsigned len, bool is_write,
                            MemTxAttrs attrs)
{
    subpage_t *subpage = opaque;
#if defined(DEBUG_SUBPAGE)
    printf("%s: subpage %p %c len %u addr " TARGET_FMT_plx "\n",
           __func__, subpage, is_write ? 'w' : 'r', len, addr);
#endif

    return flatview_access_valid(uc, subpage->fv, addr + subpage->base,
                                 len, is_write, attrs);
}