struct extent_buffer *btrfs_root_node(struct btrfs_root *root)
{
	struct extent_buffer *eb;

	while (1) {
		rcu_read_lock();
		eb = rcu_dereference(root->node);

		/*
		 * RCU really hurts here, we could free up the root node because
		 * it was COWed but we may not get the new root node yet so do
		 * the inc_not_zero dance and if it doesn't work then
		 * synchronize_rcu and try again.
		 */
		if (atomic_inc_not_zero(&eb->refs)) {
			rcu_read_unlock();
			break;
		}
		rcu_read_unlock();
		synchronize_rcu();
	}
	return eb;
}