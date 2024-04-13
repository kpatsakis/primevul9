static inline int ext3_feature_set_ok(struct super_block *sb)
{
	if (EXT4_HAS_INCOMPAT_FEATURE(sb, ~EXT3_FEATURE_INCOMPAT_SUPP))
		return 0;
	if (!EXT4_HAS_COMPAT_FEATURE(sb, EXT4_FEATURE_COMPAT_HAS_JOURNAL))
		return 0;
	if (sb->s_flags & MS_RDONLY)
		return 1;
	if (EXT4_HAS_RO_COMPAT_FEATURE(sb, ~EXT3_FEATURE_RO_COMPAT_SUPP))
		return 0;
	return 1;
}