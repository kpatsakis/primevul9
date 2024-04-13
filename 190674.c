static inline bool is_sbi_flag_set(struct f2fs_sb_info *sbi, unsigned int type)
{
	return test_bit(type, &sbi->s_flag);
}