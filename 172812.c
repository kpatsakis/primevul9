static int parse_options(char *options, struct super_block *sb,
			 unsigned long *journal_devnum,
			 unsigned int *journal_ioprio,
			 int is_remount)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	char *p, __maybe_unused *usr_qf_name, __maybe_unused *grp_qf_name;
	substring_t args[MAX_OPT_ARGS];
	int token;

	if (!options)
		return 1;

	while ((p = strsep(&options, ",")) != NULL) {
		if (!*p)
			continue;
		/*
		 * Initialize args struct so we know whether arg was
		 * found; some options take optional arguments.
		 */
		args[0].to = args[0].from = NULL;
		token = match_token(p, tokens, args);
		if (handle_mount_opt(sb, p, token, args, journal_devnum,
				     journal_ioprio, is_remount) < 0)
			return 0;
	}
#ifdef CONFIG_QUOTA
	/*
	 * We do the test below only for project quotas. 'usrquota' and
	 * 'grpquota' mount options are allowed even without quota feature
	 * to support legacy quotas in quota files.
	 */
	if (test_opt(sb, PRJQUOTA) && !ext4_has_feature_project(sb)) {
		ext4_msg(sb, KERN_ERR, "Project quota feature not enabled. "
			 "Cannot enable project quota enforcement.");
		return 0;
	}
	usr_qf_name = get_qf_name(sb, sbi, USRQUOTA);
	grp_qf_name = get_qf_name(sb, sbi, GRPQUOTA);
	if (usr_qf_name || grp_qf_name) {
		if (test_opt(sb, USRQUOTA) && usr_qf_name)
			clear_opt(sb, USRQUOTA);

		if (test_opt(sb, GRPQUOTA) && grp_qf_name)
			clear_opt(sb, GRPQUOTA);

		if (test_opt(sb, GRPQUOTA) || test_opt(sb, USRQUOTA)) {
			ext4_msg(sb, KERN_ERR, "old and new quota "
					"format mixing");
			return 0;
		}

		if (!sbi->s_jquota_fmt) {
			ext4_msg(sb, KERN_ERR, "journaled quota format "
					"not specified");
			return 0;
		}
	}
#endif
	return 1;
}