noinline void btrfs_release_path(struct btrfs_path *p)
{
	int i;

	for (i = 0; i < BTRFS_MAX_LEVEL; i++) {
		p->slots[i] = 0;
		if (!p->nodes[i])
			continue;
		if (p->locks[i]) {
			btrfs_tree_unlock_rw(p->nodes[i], p->locks[i]);
			p->locks[i] = 0;
		}
		free_extent_buffer(p->nodes[i]);
		p->nodes[i] = NULL;
	}
}