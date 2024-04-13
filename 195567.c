static void ext4_put_super(struct super_block *sb)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	int i, err;

	ext4_unregister_li_request(sb);
	dquot_disable(sb, -1, DQUOT_USAGE_ENABLED | DQUOT_LIMITS_ENABLED);

	flush_workqueue(sbi->dio_unwritten_wq);
	destroy_workqueue(sbi->dio_unwritten_wq);

	lock_super(sb);
	if (sbi->s_journal) {
		err = jbd2_journal_destroy(sbi->s_journal);
		sbi->s_journal = NULL;
		if (err < 0)
			ext4_abort(sb, "Couldn't clean up the journal");
	}

	del_timer(&sbi->s_err_report);
	ext4_release_system_zone(sb);
	ext4_mb_release(sb);
	ext4_ext_release(sb);
	ext4_xattr_put_super(sb);

	if (!(sb->s_flags & MS_RDONLY)) {
		EXT4_CLEAR_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_RECOVER);
		es->s_state = cpu_to_le16(sbi->s_mount_state);
	}
	if (sb->s_dirt || !(sb->s_flags & MS_RDONLY))
		ext4_commit_super(sb, 1);

	if (sbi->s_proc) {
		remove_proc_entry("options", sbi->s_proc);
		remove_proc_entry(sb->s_id, ext4_proc_root);
	}
	kobject_del(&sbi->s_kobj);

	for (i = 0; i < sbi->s_gdb_count; i++)
		brelse(sbi->s_group_desc[i]);
	ext4_kvfree(sbi->s_group_desc);
	ext4_kvfree(sbi->s_flex_groups);
	percpu_counter_destroy(&sbi->s_freeclusters_counter);
	percpu_counter_destroy(&sbi->s_freeinodes_counter);
	percpu_counter_destroy(&sbi->s_dirs_counter);
	percpu_counter_destroy(&sbi->s_dirtyclusters_counter);
	brelse(sbi->s_sbh);
#ifdef CONFIG_QUOTA
	for (i = 0; i < MAXQUOTAS; i++)
		kfree(sbi->s_qf_names[i]);
#endif

	/* Debugging code just in case the in-memory inode orphan list
	 * isn't empty.  The on-disk one can be non-empty if we've
	 * detected an error and taken the fs readonly, but the
	 * in-memory list had better be clean by this point. */
	if (!list_empty(&sbi->s_orphan))
		dump_orphan_list(sb, sbi);
	J_ASSERT(list_empty(&sbi->s_orphan));

	invalidate_bdev(sb->s_bdev);
	if (sbi->journal_bdev && sbi->journal_bdev != sb->s_bdev) {
		/*
		 * Invalidate the journal device's buffers.  We don't want them
		 * floating about in memory - the physical journal device may
		 * hotswapped, and it breaks the `ro-after' testing code.
		 */
		sync_blockdev(sbi->journal_bdev);
		invalidate_bdev(sbi->journal_bdev);
		ext4_blkdev_remove(sbi);
	}
	if (sbi->s_mmp_tsk)
		kthread_stop(sbi->s_mmp_tsk);
	sb->s_fs_info = NULL;
	/*
	 * Now that we are completely done shutting down the
	 * superblock, we need to actually destroy the kobject.
	 */
	unlock_super(sb);
	kobject_put(&sbi->s_kobj);
	wait_for_completion(&sbi->s_kobj_unregister);
	if (sbi->s_chksum_driver)
		crypto_free_shash(sbi->s_chksum_driver);
	kfree(sbi->s_blockgroup_lock);
	kfree(sbi);
}