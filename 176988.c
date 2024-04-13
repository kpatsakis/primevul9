static noinline int tree_mod_log_insert_move(struct extent_buffer *eb,
		int dst_slot, int src_slot, int nr_items)
{
	struct tree_mod_elem *tm = NULL;
	struct tree_mod_elem **tm_list = NULL;
	int ret = 0;
	int i;
	int locked = 0;

	if (!tree_mod_need_log(eb->fs_info, eb))
		return 0;

	tm_list = kcalloc(nr_items, sizeof(struct tree_mod_elem *), GFP_NOFS);
	if (!tm_list)
		return -ENOMEM;

	tm = kzalloc(sizeof(*tm), GFP_NOFS);
	if (!tm) {
		ret = -ENOMEM;
		goto free_tms;
	}

	tm->logical = eb->start;
	tm->slot = src_slot;
	tm->move.dst_slot = dst_slot;
	tm->move.nr_items = nr_items;
	tm->op = MOD_LOG_MOVE_KEYS;

	for (i = 0; i + dst_slot < src_slot && i < nr_items; i++) {
		tm_list[i] = alloc_tree_mod_elem(eb, i + dst_slot,
		    MOD_LOG_KEY_REMOVE_WHILE_MOVING, GFP_NOFS);
		if (!tm_list[i]) {
			ret = -ENOMEM;
			goto free_tms;
		}
	}

	if (tree_mod_dont_log(eb->fs_info, eb))
		goto free_tms;
	locked = 1;

	/*
	 * When we override something during the move, we log these removals.
	 * This can only happen when we move towards the beginning of the
	 * buffer, i.e. dst_slot < src_slot.
	 */
	for (i = 0; i + dst_slot < src_slot && i < nr_items; i++) {
		ret = __tree_mod_log_insert(eb->fs_info, tm_list[i]);
		if (ret)
			goto free_tms;
	}

	ret = __tree_mod_log_insert(eb->fs_info, tm);
	if (ret)
		goto free_tms;
	write_unlock(&eb->fs_info->tree_mod_log_lock);
	kfree(tm_list);

	return 0;
free_tms:
	for (i = 0; i < nr_items; i++) {
		if (tm_list[i] && !RB_EMPTY_NODE(&tm_list[i]->node))
			rb_erase(&tm_list[i]->node, &eb->fs_info->tree_mod_log);
		kfree(tm_list[i]);
	}
	if (locked)
		write_unlock(&eb->fs_info->tree_mod_log_lock);
	kfree(tm_list);
	kfree(tm);

	return ret;
}