static noinline int create_subvol(struct inode *dir,
				  struct dentry *dentry,
				  const char *name, int namelen,
				  u64 *async_transid,
				  struct btrfs_qgroup_inherit *inherit)
{
	struct btrfs_fs_info *fs_info = btrfs_sb(dir->i_sb);
	struct btrfs_trans_handle *trans;
	struct btrfs_key key;
	struct btrfs_root_item *root_item;
	struct btrfs_inode_item *inode_item;
	struct extent_buffer *leaf;
	struct btrfs_root *root = BTRFS_I(dir)->root;
	struct btrfs_root *new_root;
	struct btrfs_block_rsv block_rsv;
	struct timespec64 cur_time = current_time(dir);
	struct inode *inode;
	int ret;
	int err;
	u64 objectid;
	u64 new_dirid = BTRFS_FIRST_FREE_OBJECTID;
	u64 index = 0;
	uuid_le new_uuid;

	root_item = kzalloc(sizeof(*root_item), GFP_KERNEL);
	if (!root_item)
		return -ENOMEM;

	ret = btrfs_find_free_objectid(fs_info->tree_root, &objectid);
	if (ret)
		goto fail_free;

	/*
	 * Don't create subvolume whose level is not zero. Or qgroup will be
	 * screwed up since it assumes subvolume qgroup's level to be 0.
	 */
	if (btrfs_qgroup_level(objectid)) {
		ret = -ENOSPC;
		goto fail_free;
	}

	btrfs_init_block_rsv(&block_rsv, BTRFS_BLOCK_RSV_TEMP);
	/*
	 * The same as the snapshot creation, please see the comment
	 * of create_snapshot().
	 */
	ret = btrfs_subvolume_reserve_metadata(root, &block_rsv, 8, false);
	if (ret)
		goto fail_free;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		btrfs_subvolume_release_metadata(fs_info, &block_rsv);
		goto fail_free;
	}
	trans->block_rsv = &block_rsv;
	trans->bytes_reserved = block_rsv.size;

	ret = btrfs_qgroup_inherit(trans, 0, objectid, inherit);
	if (ret)
		goto fail;

	leaf = btrfs_alloc_tree_block(trans, root, 0, objectid, NULL, 0, 0, 0);
	if (IS_ERR(leaf)) {
		ret = PTR_ERR(leaf);
		goto fail;
	}

	btrfs_mark_buffer_dirty(leaf);

	inode_item = &root_item->inode;
	btrfs_set_stack_inode_generation(inode_item, 1);
	btrfs_set_stack_inode_size(inode_item, 3);
	btrfs_set_stack_inode_nlink(inode_item, 1);
	btrfs_set_stack_inode_nbytes(inode_item,
				     fs_info->nodesize);
	btrfs_set_stack_inode_mode(inode_item, S_IFDIR | 0755);

	btrfs_set_root_flags(root_item, 0);
	btrfs_set_root_limit(root_item, 0);
	btrfs_set_stack_inode_flags(inode_item, BTRFS_INODE_ROOT_ITEM_INIT);

	btrfs_set_root_bytenr(root_item, leaf->start);
	btrfs_set_root_generation(root_item, trans->transid);
	btrfs_set_root_level(root_item, 0);
	btrfs_set_root_refs(root_item, 1);
	btrfs_set_root_used(root_item, leaf->len);
	btrfs_set_root_last_snapshot(root_item, 0);

	btrfs_set_root_generation_v2(root_item,
			btrfs_root_generation(root_item));
	uuid_le_gen(&new_uuid);
	memcpy(root_item->uuid, new_uuid.b, BTRFS_UUID_SIZE);
	btrfs_set_stack_timespec_sec(&root_item->otime, cur_time.tv_sec);
	btrfs_set_stack_timespec_nsec(&root_item->otime, cur_time.tv_nsec);
	root_item->ctime = root_item->otime;
	btrfs_set_root_ctransid(root_item, trans->transid);
	btrfs_set_root_otransid(root_item, trans->transid);

	btrfs_tree_unlock(leaf);
	free_extent_buffer(leaf);
	leaf = NULL;

	btrfs_set_root_dirid(root_item, new_dirid);

	key.objectid = objectid;
	key.offset = 0;
	key.type = BTRFS_ROOT_ITEM_KEY;
	ret = btrfs_insert_root(trans, fs_info->tree_root, &key,
				root_item);
	if (ret)
		goto fail;

	key.offset = (u64)-1;
	new_root = btrfs_read_fs_root_no_name(fs_info, &key);
	if (IS_ERR(new_root)) {
		ret = PTR_ERR(new_root);
		btrfs_abort_transaction(trans, ret);
		goto fail;
	}

	btrfs_record_root_in_trans(trans, new_root);

	ret = btrfs_create_subvol_root(trans, new_root, root, new_dirid);
	if (ret) {
		/* We potentially lose an unused inode item here */
		btrfs_abort_transaction(trans, ret);
		goto fail;
	}

	mutex_lock(&new_root->objectid_mutex);
	new_root->highest_objectid = new_dirid;
	mutex_unlock(&new_root->objectid_mutex);

	/*
	 * insert the directory item
	 */
	ret = btrfs_set_inode_index(BTRFS_I(dir), &index);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		goto fail;
	}

	ret = btrfs_insert_dir_item(trans, name, namelen, BTRFS_I(dir), &key,
				    BTRFS_FT_DIR, index);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		goto fail;
	}

	btrfs_i_size_write(BTRFS_I(dir), dir->i_size + namelen * 2);
	ret = btrfs_update_inode(trans, root, dir);
	BUG_ON(ret);

	ret = btrfs_add_root_ref(trans, objectid, root->root_key.objectid,
				 btrfs_ino(BTRFS_I(dir)), index, name, namelen);
	BUG_ON(ret);

	ret = btrfs_uuid_tree_add(trans, root_item->uuid,
				  BTRFS_UUID_KEY_SUBVOL, objectid);
	if (ret)
		btrfs_abort_transaction(trans, ret);

fail:
	kfree(root_item);
	trans->block_rsv = NULL;
	trans->bytes_reserved = 0;
	btrfs_subvolume_release_metadata(fs_info, &block_rsv);

	if (async_transid) {
		*async_transid = trans->transid;
		err = btrfs_commit_transaction_async(trans, 1);
		if (err)
			err = btrfs_commit_transaction(trans);
	} else {
		err = btrfs_commit_transaction(trans);
	}
	if (err && !ret)
		ret = err;

	if (!ret) {
		inode = btrfs_lookup_dentry(dir, dentry);
		if (IS_ERR(inode))
			return PTR_ERR(inode);
		d_instantiate(dentry, inode);
	}
	return ret;

fail_free:
	kfree(root_item);
	return ret;
}