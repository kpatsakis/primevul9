static void kill_f2fs_super(struct super_block *sb)
{
	if (sb->s_root)
		set_sbi_flag(F2FS_SB(sb), SBI_IS_CLOSE);
	kill_block_super(sb);
}