static struct full_stripe_lock *insert_full_stripe_lock(
		struct btrfs_full_stripe_locks_tree *locks_root,
		u64 fstripe_logical)
{
	struct rb_node **p;
	struct rb_node *parent = NULL;
	struct full_stripe_lock *entry;
	struct full_stripe_lock *ret;

	lockdep_assert_held(&locks_root->lock);

	p = &locks_root->root.rb_node;
	while (*p) {
		parent = *p;
		entry = rb_entry(parent, struct full_stripe_lock, node);
		if (fstripe_logical < entry->logical) {
			p = &(*p)->rb_left;
		} else if (fstripe_logical > entry->logical) {
			p = &(*p)->rb_right;
		} else {
			entry->refs++;
			return entry;
		}
	}

	/*
	 * Insert new lock.
	 */
	ret = kmalloc(sizeof(*ret), GFP_KERNEL);
	if (!ret)
		return ERR_PTR(-ENOMEM);
	ret->logical = fstripe_logical;
	ret->refs = 1;
	mutex_init(&ret->mutex);

	rb_link_node(&ret->node, parent, p);
	rb_insert_color(&ret->node, &locks_root->root);
	return ret;
}