static inline void f2fs_unlock_op(struct f2fs_sb_info *sbi)
{
	up_read(&sbi->cp_rwsem);
}