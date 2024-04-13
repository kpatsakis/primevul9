static inline struct sit_info *SIT_I(struct f2fs_sb_info *sbi)
{
	return (struct sit_info *)(SM_I(sbi)->sit_info);
}