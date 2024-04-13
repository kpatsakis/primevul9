static int f2fs_remount(struct super_block *sb, int *flags, char *data)
{
	struct f2fs_sb_info *sbi = F2FS_SB(sb);
	struct f2fs_mount_info org_mount_opt;
	int err, active_logs;
	bool need_restart_gc = false;
	bool need_stop_gc = false;
	bool no_extent_cache = !test_opt(sbi, EXTENT_CACHE);

	sync_filesystem(sb);

	/*
	 * Save the old mount options in case we
	 * need to restore them.
	 */
	org_mount_opt = sbi->mount_opt;
	active_logs = sbi->active_logs;

	sbi->mount_opt.opt = 0;
	default_options(sbi);

	/* parse mount options */
	err = parse_options(sb, data);
	if (err)
		goto restore_opts;

	/*
	 * Previous and new state of filesystem is RO,
	 * so skip checking GC and FLUSH_MERGE conditions.
	 */
	if (f2fs_readonly(sb) && (*flags & MS_RDONLY))
		goto skip;

	/* disallow enable/disable extent_cache dynamically */
	if (no_extent_cache == !!test_opt(sbi, EXTENT_CACHE)) {
		err = -EINVAL;
		f2fs_msg(sbi->sb, KERN_WARNING,
				"switch extent_cache option is not allowed");
		goto restore_opts;
	}

	/*
	 * We stop the GC thread if FS is mounted as RO
	 * or if background_gc = off is passed in mount
	 * option. Also sync the filesystem.
	 */
	if ((*flags & MS_RDONLY) || !test_opt(sbi, BG_GC)) {
		if (sbi->gc_thread) {
			stop_gc_thread(sbi);
			f2fs_sync_fs(sb, 1);
			need_restart_gc = true;
		}
	} else if (!sbi->gc_thread) {
		err = start_gc_thread(sbi);
		if (err)
			goto restore_opts;
		need_stop_gc = true;
	}

	/*
	 * We stop issue flush thread if FS is mounted as RO
	 * or if flush_merge is not passed in mount option.
	 */
	if ((*flags & MS_RDONLY) || !test_opt(sbi, FLUSH_MERGE)) {
		destroy_flush_cmd_control(sbi);
	} else if (!SM_I(sbi)->cmd_control_info) {
		err = create_flush_cmd_control(sbi);
		if (err)
			goto restore_gc;
	}
skip:
	/* Update the POSIXACL Flag */
	 sb->s_flags = (sb->s_flags & ~MS_POSIXACL) |
		(test_opt(sbi, POSIX_ACL) ? MS_POSIXACL : 0);
	return 0;
restore_gc:
	if (need_restart_gc) {
		if (start_gc_thread(sbi))
			f2fs_msg(sbi->sb, KERN_WARNING,
				"background gc thread has stopped");
	} else if (need_stop_gc) {
		stop_gc_thread(sbi);
	}
restore_opts:
	sbi->mount_opt = org_mount_opt;
	sbi->active_logs = active_logs;
	return err;
}