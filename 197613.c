static uint32_t gic_compute_elrsr(GICState *s, int cpu, int lr_start)
{
    int lr_idx;
    uint32_t ret = 0;

    for (lr_idx = lr_start; lr_idx < s->num_lrs; lr_idx++) {
        uint32_t *entry = &s->h_lr[lr_idx][cpu];
        ret = deposit32(ret, lr_idx - lr_start, 1,
                        gic_lr_entry_is_free(*entry));
    }

    return ret;
}