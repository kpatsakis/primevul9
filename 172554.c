static u64 find_next_chunk(struct btrfs_fs_info *fs_info)
{
	struct extent_map_tree *em_tree;
	struct extent_map *em;
	struct rb_node *n;
	u64 ret = 0;

	em_tree = &fs_info->mapping_tree.map_tree;
	read_lock(&em_tree->lock);
	n = rb_last(&em_tree->map.rb_root);
	if (n) {
		em = rb_entry(n, struct extent_map, rb_node);
		ret = em->start + em->len;
	}
	read_unlock(&em_tree->lock);

	return ret;
}