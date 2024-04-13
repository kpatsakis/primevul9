static struct inode *f2fs_alloc_inode(struct super_block *sb)
{
	struct f2fs_inode_info *fi;

	fi = kmem_cache_alloc(f2fs_inode_cachep, GFP_F2FS_ZERO);
	if (!fi)
		return NULL;

	init_once((void *) fi);

	/* Initialize f2fs-specific inode info */
	fi->vfs_inode.i_version = 1;
	atomic_set(&fi->dirty_pages, 0);
	fi->i_current_depth = 1;
	fi->i_advise = 0;
	init_rwsem(&fi->i_sem);
	INIT_LIST_HEAD(&fi->inmem_pages);
	mutex_init(&fi->inmem_lock);

	set_inode_flag(fi, FI_NEW_INODE);

	if (test_opt(F2FS_SB(sb), INLINE_XATTR))
		set_inode_flag(fi, FI_INLINE_XATTR);

	/* Will be used by directory only */
	fi->i_dir_level = F2FS_SB(sb)->dir_level;

#ifdef CONFIG_F2FS_FS_ENCRYPTION
	fi->i_crypt_info = NULL;
#endif
	return &fi->vfs_inode;
}