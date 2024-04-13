struct extent_map *btrfs_get_chunk_map(struct btrfs_fs_info *fs_info,
				       u64 logical, u64 length)
{
	struct extent_map_tree *em_tree;
	struct extent_map *em;

	em_tree = &fs_info->mapping_tree.map_tree;
	read_lock(&em_tree->lock);
	em = lookup_extent_mapping(em_tree, logical, length);
	read_unlock(&em_tree->lock);

	if (!em) {
		btrfs_crit(fs_info, "unable to find logical %llu length %llu",
			   logical, length);
		return ERR_PTR(-EINVAL);
	}

	if (em->start > logical || em->start + em->len < logical) {
		btrfs_crit(fs_info,
			   "found a bad mapping, wanted %llu-%llu, found %llu-%llu",
			   logical, length, em->start, em->start + em->len);
		free_extent_map(em);
		return ERR_PTR(-EINVAL);
	}

	/* callers are responsible for dropping em's ref. */
	return em;
}