static inline void f2fs_lock_op(struct f2fs_sb_info *sbi)
{
	down_read(&sbi->cp_rwsem);
}