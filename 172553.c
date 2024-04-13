static struct full_stripe_lock *search_full_stripe_lock(
		struct btrfs_full_stripe_locks_tree *locks_root,
		u64 fstripe_logical)
{
	struct rb_node *node;
	struct full_stripe_lock *entry;

	lockdep_assert_held(&locks_root->lock);

	node = locks_root->root.rb_node;
	while (node) {
		entry = rb_entry(node, struct full_stripe_lock, node);
		if (fstripe_logical < entry->logical)
			node = node->rb_left;
		else if (fstripe_logical > entry->logical)
			node = node->rb_right;
		else
			return entry;
	}
	return NULL;
}