static bool check_sibling_keys(struct extent_buffer *left,
			       struct extent_buffer *right)
{
	struct btrfs_key left_last;
	struct btrfs_key right_first;
	int level = btrfs_header_level(left);
	int nr_left = btrfs_header_nritems(left);
	int nr_right = btrfs_header_nritems(right);

	/* No key to check in one of the tree blocks */
	if (!nr_left || !nr_right)
		return false;

	if (level) {
		btrfs_node_key_to_cpu(left, &left_last, nr_left - 1);
		btrfs_node_key_to_cpu(right, &right_first, 0);
	} else {
		btrfs_item_key_to_cpu(left, &left_last, nr_left - 1);
		btrfs_item_key_to_cpu(right, &right_first, 0);
	}

	if (btrfs_comp_cpu_keys(&left_last, &right_first) >= 0) {
		btrfs_crit(left->fs_info,
"bad key order, sibling blocks, left last (%llu %u %llu) right first (%llu %u %llu)",
			   left_last.objectid, left_last.type,
			   left_last.offset, right_first.objectid,
			   right_first.type, right_first.offset);
		return true;
	}
	return false;
}