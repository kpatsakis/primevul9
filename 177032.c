static struct tree_mod_elem *__tree_mod_log_oldest_root(
		struct extent_buffer *eb_root, u64 time_seq)
{
	struct tree_mod_elem *tm;
	struct tree_mod_elem *found = NULL;
	u64 root_logical = eb_root->start;
	int looped = 0;

	if (!time_seq)
		return NULL;

	/*
	 * the very last operation that's logged for a root is the
	 * replacement operation (if it is replaced at all). this has
	 * the logical address of the *new* root, making it the very
	 * first operation that's logged for this root.
	 */
	while (1) {
		tm = tree_mod_log_search_oldest(eb_root->fs_info, root_logical,
						time_seq);
		if (!looped && !tm)
			return NULL;
		/*
		 * if there are no tree operation for the oldest root, we simply
		 * return it. this should only happen if that (old) root is at
		 * level 0.
		 */
		if (!tm)
			break;

		/*
		 * if there's an operation that's not a root replacement, we
		 * found the oldest version of our root. normally, we'll find a
		 * MOD_LOG_KEY_REMOVE_WHILE_FREEING operation here.
		 */
		if (tm->op != MOD_LOG_ROOT_REPLACE)
			break;

		found = tm;
		root_logical = tm->old_root.logical;
		looped = 1;
	}

	/* if there's no old root to return, return what we found instead */
	if (!found)
		found = tm;

	return found;
}