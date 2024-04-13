static noinline int insert_new_root(struct btrfs_trans_handle *trans,
			   struct btrfs_root *root,
			   struct btrfs_path *path, int level)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	u64 lower_gen;
	struct extent_buffer *lower;
	struct extent_buffer *c;
	struct extent_buffer *old;
	struct btrfs_disk_key lower_key;
	int ret;

	BUG_ON(path->nodes[level]);
	BUG_ON(path->nodes[level-1] != root->node);

	lower = path->nodes[level-1];
	if (level == 1)
		btrfs_item_key(lower, &lower_key, 0);
	else
		btrfs_node_key(lower, &lower_key, 0);

	c = alloc_tree_block_no_bg_flush(trans, root, 0, &lower_key, level,
					 root->node->start, 0,
					 BTRFS_NESTING_NEW_ROOT);
	if (IS_ERR(c))
		return PTR_ERR(c);

	root_add_used(root, fs_info->nodesize);

	btrfs_set_header_nritems(c, 1);
	btrfs_set_node_key(c, &lower_key, 0);
	btrfs_set_node_blockptr(c, 0, lower->start);
	lower_gen = btrfs_header_generation(lower);
	WARN_ON(lower_gen != trans->transid);

	btrfs_set_node_ptr_generation(c, 0, lower_gen);

	btrfs_mark_buffer_dirty(c);

	old = root->node;
	ret = tree_mod_log_insert_root(root->node, c, 0);
	BUG_ON(ret < 0);
	rcu_assign_pointer(root->node, c);

	/* the super has an extra ref to root->node */
	free_extent_buffer(old);

	add_root_to_dirty_list(root);
	atomic_inc(&c->refs);
	path->nodes[level] = c;
	path->locks[level] = BTRFS_WRITE_LOCK;
	path->slots[level] = 0;
	return 0;
}