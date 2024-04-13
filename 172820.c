static inline char *get_qf_name(struct super_block *sb,
				struct ext4_sb_info *sbi,
				int type)
{
	return rcu_dereference_protected(sbi->s_qf_names[type],
					 lockdep_is_held(&sb->s_umount));
}