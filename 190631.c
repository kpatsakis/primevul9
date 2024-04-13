static inline void *f2fs_kmalloc(struct f2fs_sb_info *sbi,
					size_t size, gfp_t flags)
{
	void *ret;

	if (time_to_inject(sbi, FAULT_KMALLOC)) {
		f2fs_show_injection_info(FAULT_KMALLOC);
		return NULL;
	}

	ret = kmalloc(size, flags);
	if (ret)
		return ret;

	return kvmalloc(size, flags);
}