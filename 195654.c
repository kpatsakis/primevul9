static ext4_fsblk_t descriptor_loc(struct super_block *sb,
				   ext4_fsblk_t logical_sb_block, int nr)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	ext4_group_t bg, first_meta_bg;
	int has_super = 0;

	first_meta_bg = le32_to_cpu(sbi->s_es->s_first_meta_bg);

	if (!EXT4_HAS_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_META_BG) ||
	    nr < first_meta_bg)
		return logical_sb_block + nr + 1;
	bg = sbi->s_desc_per_block * nr;
	if (ext4_bg_has_super(sb, bg))
		has_super = 1;

	return (has_super + ext4_group_first_block_no(sb, bg));
}