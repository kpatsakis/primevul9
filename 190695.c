static inline void *f2fs_kvzalloc(struct f2fs_sb_info *sbi,
					size_t size, gfp_t flags)
{
	return f2fs_kvmalloc(sbi, size, flags | __GFP_ZERO);
}