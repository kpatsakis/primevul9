static inline void f2fs_unlock_all(struct f2fs_sb_info *sbi)
{
	up_write(&sbi->cp_rwsem);
}