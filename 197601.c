static inline bool gic_lr_entry_is_eoi(uint32_t entry)
{
    return (GICH_LR_STATE(entry) == GICH_LR_STATE_INVALID)
        && !GICH_LR_HW(entry) && GICH_LR_EOI(entry);
}