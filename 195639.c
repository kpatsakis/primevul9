static int handle_mount_opt(struct super_block *sb, char *opt, int token,
			    substring_t *args, unsigned long *journal_devnum,
			    unsigned int *journal_ioprio, int is_remount)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	const struct mount_opts *m;
	kuid_t uid;
	kgid_t gid;
	int arg = 0;

#ifdef CONFIG_QUOTA
	if (token == Opt_usrjquota)
		return set_qf_name(sb, USRQUOTA, &args[0]);
	else if (token == Opt_grpjquota)
		return set_qf_name(sb, GRPQUOTA, &args[0]);
	else if (token == Opt_offusrjquota)
		return clear_qf_name(sb, USRQUOTA);
	else if (token == Opt_offgrpjquota)
		return clear_qf_name(sb, GRPQUOTA);
#endif
	if (args->from && match_int(args, &arg))
		return -1;
	switch (token) {
	case Opt_noacl:
	case Opt_nouser_xattr:
		ext4_msg(sb, KERN_WARNING, deprecated_msg, opt, "3.5");
		break;
	case Opt_sb:
		return 1;	/* handled by get_sb_block() */
	case Opt_removed:
		ext4_msg(sb, KERN_WARNING,
			 "Ignoring removed %s option", opt);
		return 1;
	case Opt_resuid:
		uid = make_kuid(current_user_ns(), arg);
		if (!uid_valid(uid)) {
			ext4_msg(sb, KERN_ERR, "Invalid uid value %d", arg);
			return -1;
		}
		sbi->s_resuid = uid;
		return 1;
	case Opt_resgid:
		gid = make_kgid(current_user_ns(), arg);
		if (!gid_valid(gid)) {
			ext4_msg(sb, KERN_ERR, "Invalid gid value %d", arg);
			return -1;
		}
		sbi->s_resgid = gid;
		return 1;
	case Opt_abort:
		sbi->s_mount_flags |= EXT4_MF_FS_ABORTED;
		return 1;
	case Opt_i_version:
		sb->s_flags |= MS_I_VERSION;
		return 1;
	case Opt_journal_dev:
		if (is_remount) {
			ext4_msg(sb, KERN_ERR,
				 "Cannot specify journal on remount");
			return -1;
		}
		*journal_devnum = arg;
		return 1;
	case Opt_journal_ioprio:
		if (arg < 0 || arg > 7)
			return -1;
		*journal_ioprio = IOPRIO_PRIO_VALUE(IOPRIO_CLASS_BE, arg);
		return 1;
	}

	for (m = ext4_mount_opts; m->token != Opt_err; m++) {
		if (token != m->token)
			continue;
		if (args->from && (m->flags & MOPT_GTE0) && (arg < 0))
			return -1;
		if (m->flags & MOPT_EXPLICIT)
			set_opt2(sb, EXPLICIT_DELALLOC);
		if (m->flags & MOPT_CLEAR_ERR)
			clear_opt(sb, ERRORS_MASK);
		if (token == Opt_noquota && sb_any_quota_loaded(sb)) {
			ext4_msg(sb, KERN_ERR, "Cannot change quota "
				 "options when quota turned on");
			return -1;
		}

		if (m->flags & MOPT_NOSUPPORT) {
			ext4_msg(sb, KERN_ERR, "%s option not supported", opt);
		} else if (token == Opt_commit) {
			if (arg == 0)
				arg = JBD2_DEFAULT_MAX_COMMIT_AGE;
			sbi->s_commit_interval = HZ * arg;
		} else if (token == Opt_max_batch_time) {
			if (arg == 0)
				arg = EXT4_DEF_MAX_BATCH_TIME;
			sbi->s_max_batch_time = arg;
		} else if (token == Opt_min_batch_time) {
			sbi->s_min_batch_time = arg;
		} else if (token == Opt_inode_readahead_blks) {
			if (arg > (1 << 30))
				return -1;
			if (arg && !is_power_of_2(arg)) {
				ext4_msg(sb, KERN_ERR,
					 "EXT4-fs: inode_readahead_blks"
					 " must be a power of 2");
				return -1;
			}
			sbi->s_inode_readahead_blks = arg;
		} else if (token == Opt_init_itable) {
			set_opt(sb, INIT_INODE_TABLE);
			if (!args->from)
				arg = EXT4_DEF_LI_WAIT_MULT;
			sbi->s_li_wait_mult = arg;
		} else if (token == Opt_stripe) {
			sbi->s_stripe = arg;
		} else if (m->flags & MOPT_DATAJ) {
			if (is_remount) {
				if (!sbi->s_journal)
					ext4_msg(sb, KERN_WARNING, "Remounting file system with no journal so ignoring journalled data option");
				else if (test_opt(sb, DATA_FLAGS) !=
					 m->mount_opt) {
					ext4_msg(sb, KERN_ERR,
					 "Cannot change data mode on remount");
					return -1;
				}
			} else {
				clear_opt(sb, DATA_FLAGS);
				sbi->s_mount_opt |= m->mount_opt;
			}
#ifdef CONFIG_QUOTA
		} else if (m->flags & MOPT_QFMT) {
			if (sb_any_quota_loaded(sb) &&
			    sbi->s_jquota_fmt != m->mount_opt) {
				ext4_msg(sb, KERN_ERR, "Cannot "
					 "change journaled quota options "
					 "when quota turned on");
				return -1;
			}
			sbi->s_jquota_fmt = m->mount_opt;
#endif
		} else {
			if (!args->from)
				arg = 1;
			if (m->flags & MOPT_CLEAR)
				arg = !arg;
			else if (unlikely(!(m->flags & MOPT_SET))) {
				ext4_msg(sb, KERN_WARNING,
					 "buggy handling of option %s", opt);
				WARN_ON(1);
				return -1;
			}
			if (arg != 0)
				sbi->s_mount_opt |= m->mount_opt;
			else
				sbi->s_mount_opt &= ~m->mount_opt;
		}
		return 1;
	}
	ext4_msg(sb, KERN_ERR, "Unrecognized mount option \"%s\" "
		 "or missing value", opt);
	return -1;
}