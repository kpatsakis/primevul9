static inline void *f2fs_kvmalloc(struct f2fs_sb_info *sbi,
					size_t size, gfp_t flags)
{
	if (time_to_inject(sbi, FAULT_KVMALLOC)) {
		f2fs_show_injection_info(FAULT_KVMALLOC);
		return NULL;
	}

	return kvmalloc(size, flags);
}