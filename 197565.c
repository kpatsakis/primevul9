static inline void gic_get_best_virq(GICState *s, int cpu,
                                     int *best_irq, int *best_prio, int *group)
{
    int lr_idx = 0;

    *best_irq = 1023;
    *best_prio = 0x100;

    for (lr_idx = 0; lr_idx < s->num_lrs; lr_idx++) {
        uint32_t lr_entry = s->h_lr[lr_idx][cpu];
        int state = GICH_LR_STATE(lr_entry);

        if (state == GICH_LR_STATE_PENDING) {
            int prio = GICH_LR_PRIORITY(lr_entry);

            if (prio < *best_prio) {
                *best_prio = prio;
                *best_irq = GICH_LR_VIRT_ID(lr_entry);
                *group = GICH_LR_GROUP(lr_entry);
            }
        }
    }
}