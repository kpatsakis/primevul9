tree_mod_log_rewind(struct btrfs_fs_info *fs_info, struct btrfs_path *path,
		    struct extent_buffer *eb, u64 time_seq)
{
	struct extent_buffer *eb_rewin;
	struct tree_mod_elem *tm;

	if (!time_seq)
		return eb;

	if (btrfs_header_level(eb) == 0)
		return eb;

	tm = tree_mod_log_search(fs_info, eb->start, time_seq);
	if (!tm)
		return eb;

	if (tm->op == MOD_LOG_KEY_REMOVE_WHILE_FREEING) {
		BUG_ON(tm->slot != 0);
		eb_rewin = alloc_dummy_extent_buffer(fs_info, eb->start);
		if (!eb_rewin) {
			btrfs_tree_read_unlock(eb);
			free_extent_buffer(eb);
			return NULL;
		}
		btrfs_set_header_bytenr(eb_rewin, eb->start);
		btrfs_set_header_backref_rev(eb_rewin,
					     btrfs_header_backref_rev(eb));
		btrfs_set_header_owner(eb_rewin, btrfs_header_owner(eb));
		btrfs_set_header_level(eb_rewin, btrfs_header_level(eb));
	} else {
		eb_rewin = btrfs_clone_extent_buffer(eb);
		if (!eb_rewin) {
			btrfs_tree_read_unlock(eb);
			free_extent_buffer(eb);
			return NULL;
		}
	}

	btrfs_tree_read_unlock(eb);
	free_extent_buffer(eb);

	btrfs_set_buffer_lockdep_class(btrfs_header_owner(eb_rewin),
				       eb_rewin, btrfs_header_level(eb_rewin));
	btrfs_tree_read_lock(eb_rewin);
	__tree_mod_log_rewind(fs_info, eb_rewin, time_seq, tm);
	WARN_ON(btrfs_header_nritems(eb_rewin) >
		BTRFS_NODEPTRS_PER_BLOCK(fs_info));

	return eb_rewin;
}