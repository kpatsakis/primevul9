static bool detect_write_flooding(struct kvm_mmu_page *sp)
{
	/*
	 * Skip write-flooding detected for the sp whose level is 1, because
	 * it can become unsync, then the guest page is not write-protected.
	 */
	if (sp->role.level == PG_LEVEL_4K)
		return false;

	atomic_inc(&sp->write_flooding_count);
	return atomic_read(&sp->write_flooding_count) >= 3;
}