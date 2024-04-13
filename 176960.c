static noinline int tree_mod_log_insert_key(struct extent_buffer *eb, int slot,
		enum mod_log_op op, gfp_t flags)
{
	struct tree_mod_elem *tm;
	int ret;

	if (!tree_mod_need_log(eb->fs_info, eb))
		return 0;

	tm = alloc_tree_mod_elem(eb, slot, op, flags);
	if (!tm)
		return -ENOMEM;

	if (tree_mod_dont_log(eb->fs_info, eb)) {
		kfree(tm);
		return 0;
	}

	ret = __tree_mod_log_insert(eb->fs_info, tm);
	write_unlock(&eb->fs_info->tree_mod_log_lock);
	if (ret)
		kfree(tm);

	return ret;
}