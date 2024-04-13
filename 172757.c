static int set_qf_name(struct super_block *sb, int qtype, substring_t *args)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	char *qname, *old_qname = get_qf_name(sb, sbi, qtype);
	int ret = -1;

	if (sb_any_quota_loaded(sb) && !old_qname) {
		ext4_msg(sb, KERN_ERR,
			"Cannot change journaled "
			"quota options when quota turned on");
		return -1;
	}
	if (ext4_has_feature_quota(sb)) {
		ext4_msg(sb, KERN_INFO, "Journaled quota options "
			 "ignored when QUOTA feature is enabled");
		return 1;
	}
	qname = match_strdup(args);
	if (!qname) {
		ext4_msg(sb, KERN_ERR,
			"Not enough memory for storing quotafile name");
		return -1;
	}
	if (old_qname) {
		if (strcmp(old_qname, qname) == 0)
			ret = 1;
		else
			ext4_msg(sb, KERN_ERR,
				 "%s quota file already specified",
				 QTYPE2NAME(qtype));
		goto errout;
	}
	if (strchr(qname, '/')) {
		ext4_msg(sb, KERN_ERR,
			"quotafile must be on filesystem root");
		goto errout;
	}
	rcu_assign_pointer(sbi->s_qf_names[qtype], qname);
	set_opt(sb, QUOTA);
	return 1;
errout:
	kfree(qname);
	return ret;
}