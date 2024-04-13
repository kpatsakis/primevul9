static int ext4_quota_on_mount(struct super_block *sb, int type)
{
	return dquot_quota_on_mount(sb, get_qf_name(sb, EXT4_SB(sb), type),
					EXT4_SB(sb)->s_jquota_fmt, type);
}