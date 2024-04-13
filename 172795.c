static void ext4_put_super(struct super_block *sb)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	int aborted = 0;
	int i, err;

	ext4_unregister_li_request(sb);
	ext4_quota_off_umount(sb);

	destroy_workqueue(sbi->rsv_conversion_wq);

	if (sbi->s_journal) {
		aborted = is_journal_aborted(sbi->s_journal);
		err = jbd2_journal_destroy(sbi->s_journal);
		sbi->s_journal = NULL;
		if ((err < 0) && !aborted)
			ext4_abort(sb, "Couldn't clean up the journal");
	}

	ext4_unregister_sysfs(sb);
	ext4_es_unregister_shrinker(sbi);
	del_timer_sync(&sbi->s_err_report);
	ext4_release_system_zone(sb);
	ext4_mb_release(sb);
	ext4_ext_release(sb);

	if (!sb_rdonly(sb) && !aborted) {
		ext4_clear_feature_journal_needs_recovery(sb);
		es->s_state = cpu_to_le16(sbi->s_mount_state);
	}
	if (!sb_rdonly(sb))
		ext4_commit_super(sb, 1);

	for (i = 0; i < sbi->s_gdb_count; i++)
		brelse(sbi->s_group_desc[i]);
	kvfree(sbi->s_group_desc);
	kvfree(sbi->s_flex_groups);
	percpu_counter_destroy(&sbi->s_freeclusters_counter);
	percpu_counter_destroy(&sbi->s_freeinodes_counter);
	percpu_counter_destroy(&sbi->s_dirs_counter);
	percpu_counter_destroy(&sbi->s_dirtyclusters_counter);
	percpu_free_rwsem(&sbi->s_journal_flag_rwsem);
#ifdef CONFIG_QUOTA
	for (i = 0; i < EXT4_MAXQUOTAS; i++)
		kfree(get_qf_name(sb, sbi, i));
#endif

	/* Debugging code just in case the in-memory inode orphan list
	 * isn't empty.  The on-disk one can be non-empty if we've
	 * detected an error and taken the fs readonly, but the
	 * in-memory list had better be clean by this point. */
	if (!list_empty(&sbi->s_orphan))
		dump_orphan_list(sb, sbi);
	J_ASSERT(list_empty(&sbi->s_orphan));

	sync_blockdev(sb->s_bdev);
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

	ext4_xattr_destroy_cache(sbi->s_ea_inode_cache);
	sbi->s_ea_inode_cache = NULL;

	ext4_xattr_destroy_cache(sbi->s_ea_block_cache);
	sbi->s_ea_block_cache = NULL;

	if (sbi->s_mmp_tsk)
		kthread_stop(sbi->s_mmp_tsk);
	brelse(sbi->s_sbh);
	sb->s_fs_info = NULL;
	/*
	 * Now that we are completely done shutting down the
	 * superblock, we need to actually destroy the kobject.
	 */
	kobject_put(&sbi->s_kobj);
	wait_for_completion(&sbi->s_kobj_unregister);
	if (sbi->s_chksum_driver)
		crypto_free_shash(sbi->s_chksum_driver);
	kfree(sbi->s_blockgroup_lock);
	fs_put_dax(sbi->s_daxdev);
#ifdef CONFIG_UNICODE
	utf8_unload(sbi->s_encoding);
#endif
	kfree(sbi);
}