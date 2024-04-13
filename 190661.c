static inline void clear_sbi_flag(struct f2fs_sb_info *sbi, unsigned int type)
{
	clear_bit(type, &sbi->s_flag);
}