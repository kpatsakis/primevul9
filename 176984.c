static noinline int tree_mod_log_eb_copy(struct extent_buffer *dst,
		     struct extent_buffer *src, unsigned long dst_offset,
		     unsigned long src_offset, int nr_items)
{
	struct btrfs_fs_info *fs_info = dst->fs_info;
	int ret = 0;
	struct tree_mod_elem **tm_list = NULL;
	struct tree_mod_elem **tm_list_add, **tm_list_rem;
	int i;
	int locked = 0;

	if (!tree_mod_need_log(fs_info, NULL))
		return 0;

	if (btrfs_header_level(dst) == 0 && btrfs_header_level(src) == 0)
		return 0;

	tm_list = kcalloc(nr_items * 2, sizeof(struct tree_mod_elem *),
			  GFP_NOFS);
	if (!tm_list)
		return -ENOMEM;

	tm_list_add = tm_list;
	tm_list_rem = tm_list + nr_items;
	for (i = 0; i < nr_items; i++) {
		tm_list_rem[i] = alloc_tree_mod_elem(src, i + src_offset,
		    MOD_LOG_KEY_REMOVE, GFP_NOFS);
		if (!tm_list_rem[i]) {
			ret = -ENOMEM;
			goto free_tms;
		}

		tm_list_add[i] = alloc_tree_mod_elem(dst, i + dst_offset,
		    MOD_LOG_KEY_ADD, GFP_NOFS);
		if (!tm_list_add[i]) {
			ret = -ENOMEM;
			goto free_tms;
		}
	}

	if (tree_mod_dont_log(fs_info, NULL))
		goto free_tms;
	locked = 1;

	for (i = 0; i < nr_items; i++) {
		ret = __tree_mod_log_insert(fs_info, tm_list_rem[i]);
		if (ret)
			goto free_tms;
		ret = __tree_mod_log_insert(fs_info, tm_list_add[i]);
		if (ret)
			goto free_tms;
	}

	write_unlock(&fs_info->tree_mod_log_lock);
	kfree(tm_list);

	return 0;

free_tms:
	for (i = 0; i < nr_items * 2; i++) {
		if (tm_list[i] && !RB_EMPTY_NODE(&tm_list[i]->node))
			rb_erase(&tm_list[i]->node, &fs_info->tree_mod_log);
		kfree(tm_list[i]);
	}
	if (locked)
		write_unlock(&fs_info->tree_mod_log_lock);
	kfree(tm_list);

	return ret;
}