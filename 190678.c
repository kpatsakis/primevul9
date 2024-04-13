static inline void f2fs_lock_all(struct f2fs_sb_info *sbi)
{
	down_write(&sbi->cp_rwsem);
}