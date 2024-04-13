static int parse_options(char *options, struct super_block *sb,
			 unsigned long *journal_devnum,
			 unsigned int *journal_ioprio,
			 int is_remount)
{
#ifdef CONFIG_QUOTA
	struct ext4_sb_info *sbi = EXT4_SB(sb);
#endif
	char *p;
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
		args[0].to = args[0].from = 0;
		token = match_token(p, tokens, args);
		if (handle_mount_opt(sb, p, token, args, journal_devnum,
				     journal_ioprio, is_remount) < 0)
			return 0;
	}
#ifdef CONFIG_QUOTA
	if (sbi->s_qf_names[USRQUOTA] || sbi->s_qf_names[GRPQUOTA]) {
		if (test_opt(sb, USRQUOTA) && sbi->s_qf_names[USRQUOTA])
			clear_opt(sb, USRQUOTA);

		if (test_opt(sb, GRPQUOTA) && sbi->s_qf_names[GRPQUOTA])
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
	} else {
		if (sbi->s_jquota_fmt) {
			ext4_msg(sb, KERN_ERR, "journaled quota format "
					"specified with no journaling "
					"enabled");
			return 0;
		}
	}
#endif
	return 1;
}