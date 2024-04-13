static inline void disable_nat_bits(struct f2fs_sb_info *sbi, bool lock)
{
	unsigned long flags;

	/*
	 * In order to re-enable nat_bits we need to call fsck.f2fs by
	 * set_sbi_flag(sbi, SBI_NEED_FSCK). But it may give huge cost,
	 * so let's rely on regular fsck or unclean shutdown.
	 */

	if (lock)
		spin_lock_irqsave(&sbi->cp_lock, flags);
	__clear_ckpt_flags(F2FS_CKPT(sbi), CP_NAT_BITS_FLAG);
	kvfree(NM_I(sbi)->nat_bits);
	NM_I(sbi)->nat_bits = NULL;
	if (lock)
		spin_unlock_irqrestore(&sbi->cp_lock, flags);
}