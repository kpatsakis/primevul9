static void f2fs_put_super(struct super_block *sb)
{
	struct f2fs_sb_info *sbi = F2FS_SB(sb);

	if (sbi->s_proc) {
		remove_proc_entry("segment_info", sbi->s_proc);
		remove_proc_entry(sb->s_id, f2fs_proc_root);
	}
	kobject_del(&sbi->s_kobj);

	stop_gc_thread(sbi);

	/* prevent remaining shrinker jobs */
	mutex_lock(&sbi->umount_mutex);

	/*
	 * We don't need to do checkpoint when superblock is clean.
	 * But, the previous checkpoint was not done by umount, it needs to do
	 * clean checkpoint again.
	 */
	if (is_sbi_flag_set(sbi, SBI_IS_DIRTY) ||
			!is_set_ckpt_flags(F2FS_CKPT(sbi), CP_UMOUNT_FLAG)) {
		struct cp_control cpc = {
			.reason = CP_UMOUNT,
		};
		write_checkpoint(sbi, &cpc);
	}

	/* write_checkpoint can update stat informaion */
	f2fs_destroy_stats(sbi);

	/*
	 * normally superblock is clean, so we need to release this.
	 * In addition, EIO will skip do checkpoint, we need this as well.
	 */
	release_dirty_inode(sbi);
	release_discard_addrs(sbi);

	f2fs_leave_shrinker(sbi);
	mutex_unlock(&sbi->umount_mutex);

	iput(sbi->node_inode);
	iput(sbi->meta_inode);

	/* destroy f2fs internal modules */
	destroy_node_manager(sbi);
	destroy_segment_manager(sbi);

	kfree(sbi->ckpt);
	kobject_put(&sbi->s_kobj);
	wait_for_completion(&sbi->s_kobj_unregister);

	sb->s_fs_info = NULL;
	brelse(sbi->raw_super_buf);
	kfree(sbi);
}