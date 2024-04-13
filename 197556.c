static inline void gic_extract_lr_info(GICState *s, int cpu,
                                int *num_eoi, int *num_valid, int *num_pending)
{
    int lr_idx;

    *num_eoi = 0;
    *num_valid = 0;
    *num_pending = 0;

    for (lr_idx = 0; lr_idx < s->num_lrs; lr_idx++) {
        uint32_t *entry = &s->h_lr[lr_idx][cpu];

        if (gic_lr_entry_is_eoi(*entry)) {
            (*num_eoi)++;
        }

        if (GICH_LR_STATE(*entry) != GICH_LR_STATE_INVALID) {
            (*num_valid)++;
        }

        if (GICH_LR_STATE(*entry) == GICH_LR_STATE_PENDING) {
            (*num_pending)++;
        }
    }
}