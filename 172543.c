static int verify_chunk_dev_extent_mapping(struct btrfs_fs_info *fs_info)
{
	struct extent_map_tree *em_tree = &fs_info->mapping_tree.map_tree;
	struct extent_map *em;
	struct rb_node *node;
	int ret = 0;

	read_lock(&em_tree->lock);
	for (node = rb_first_cached(&em_tree->map); node; node = rb_next(node)) {
		em = rb_entry(node, struct extent_map, rb_node);
		if (em->map_lookup->num_stripes !=
		    em->map_lookup->verified_stripes) {
			btrfs_err(fs_info,
			"chunk %llu has missing dev extent, have %d expect %d",
				  em->start, em->map_lookup->verified_stripes,
				  em->map_lookup->num_stripes);
			ret = -EUCLEAN;
			goto out;
		}
	}
out:
	read_unlock(&em_tree->lock);
	return ret;
}