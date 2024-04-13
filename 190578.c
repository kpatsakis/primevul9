static inline bool f2fs_cp_error(struct f2fs_sb_info *sbi)
{
	return is_set_ckpt_flags(sbi, CP_ERROR_FLAG);
}