static struct inode *ext4_alloc_inode(struct super_block *sb)
{
	struct ext4_inode_info *ei;

	ei = kmem_cache_alloc(ext4_inode_cachep, GFP_NOFS);
	if (!ei)
		return NULL;

	ei->vfs_inode.i_version = 1;
	ei->vfs_inode.i_data.writeback_index = 0;
	memset(&ei->i_cached_extent, 0, sizeof(struct ext4_ext_cache));
	INIT_LIST_HEAD(&ei->i_prealloc_list);
	spin_lock_init(&ei->i_prealloc_lock);
	ei->i_reserved_data_blocks = 0;
	ei->i_reserved_meta_blocks = 0;
	ei->i_allocated_meta_blocks = 0;
	ei->i_da_metadata_calc_len = 0;
	spin_lock_init(&(ei->i_block_reservation_lock));
#ifdef CONFIG_QUOTA
	ei->i_reserved_quota = 0;
#endif
	ei->jinode = NULL;
	INIT_LIST_HEAD(&ei->i_completed_io_list);
	spin_lock_init(&ei->i_completed_io_lock);
	ei->cur_aio_dio = NULL;
	ei->i_sync_tid = 0;
	ei->i_datasync_tid = 0;
	atomic_set(&ei->i_ioend_count, 0);
	atomic_set(&ei->i_aiodio_unwritten, 0);

	return &ei->vfs_inode;
}