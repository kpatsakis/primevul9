static inline int f2fs_trylock_op(struct f2fs_sb_info *sbi)
{
	return down_read_trylock(&sbi->cp_rwsem);
}