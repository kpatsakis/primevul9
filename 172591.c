void btrfs_mapping_init(struct btrfs_mapping_tree *tree)
{
	extent_map_tree_init(&tree->map_tree);
}