static inline bool f2fs_readonly(struct super_block *sb)
{
	return sb_rdonly(sb);
}