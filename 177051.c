static noinline int tree_mod_log_free_eb(struct extent_buffer *eb)
{
	struct tree_mod_elem **tm_list = NULL;
	int nritems = 0;
	int i;
	int ret = 0;

	if (btrfs_header_level(eb) == 0)
		return 0;

	if (!tree_mod_need_log(eb->fs_info, NULL))
		return 0;

	nritems = btrfs_header_nritems(eb);
	tm_list = kcalloc(nritems, sizeof(struct tree_mod_elem *), GFP_NOFS);
	if (!tm_list)
		return -ENOMEM;

	for (i = 0; i < nritems; i++) {
		tm_list[i] = alloc_tree_mod_elem(eb, i,
		    MOD_LOG_KEY_REMOVE_WHILE_FREEING, GFP_NOFS);
		if (!tm_list[i]) {
			ret = -ENOMEM;
			goto free_tms;
		}
	}

	if (tree_mod_dont_log(eb->fs_info, eb))
		goto free_tms;

	ret = __tree_mod_log_free_eb(eb->fs_info, tm_list, nritems);
	write_unlock(&eb->fs_info->tree_mod_log_lock);
	if (ret)
		goto free_tms;
	kfree(tm_list);

	return 0;

free_tms:
	for (i = 0; i < nritems; i++)
		kfree(tm_list[i]);
	kfree(tm_list);

	return ret;
}