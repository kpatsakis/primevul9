static void ext4_clamp_want_extra_isize(struct super_block *sb)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	unsigned def_extra_isize = sizeof(struct ext4_inode) -
						EXT4_GOOD_OLD_INODE_SIZE;

	if (sbi->s_inode_size == EXT4_GOOD_OLD_INODE_SIZE) {
		sbi->s_want_extra_isize = 0;
		return;
	}
	if (sbi->s_want_extra_isize < 4) {
		sbi->s_want_extra_isize = def_extra_isize;
		if (ext4_has_feature_extra_isize(sb)) {
			if (sbi->s_want_extra_isize <
			    le16_to_cpu(es->s_want_extra_isize))
				sbi->s_want_extra_isize =
					le16_to_cpu(es->s_want_extra_isize);
			if (sbi->s_want_extra_isize <
			    le16_to_cpu(es->s_min_extra_isize))
				sbi->s_want_extra_isize =
					le16_to_cpu(es->s_min_extra_isize);
		}
	}
	/* Check if enough inode space is available */
	if ((sbi->s_want_extra_isize > sbi->s_inode_size) ||
	    (EXT4_GOOD_OLD_INODE_SIZE + sbi->s_want_extra_isize >
							sbi->s_inode_size)) {
		sbi->s_want_extra_isize = def_extra_isize;
		ext4_msg(sb, KERN_INFO,
			 "required extra inode space not available");
	}
}