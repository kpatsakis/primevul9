static inline void set_sbi_flag(struct f2fs_sb_info *sbi, unsigned int type)
{
	set_bit(type, &sbi->s_flag);
}