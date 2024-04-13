static void reada_for_search(struct btrfs_fs_info *fs_info,
			     struct btrfs_path *path,
			     int level, int slot, u64 objectid)
{
	struct extent_buffer *node;
	struct btrfs_disk_key disk_key;
	u32 nritems;
	u64 search;
	u64 target;
	u64 nread = 0;
	struct extent_buffer *eb;
	u32 nr;
	u32 blocksize;
	u32 nscan = 0;

	if (level != 1)
		return;

	if (!path->nodes[level])
		return;

	node = path->nodes[level];

	search = btrfs_node_blockptr(node, slot);
	blocksize = fs_info->nodesize;
	eb = find_extent_buffer(fs_info, search);
	if (eb) {
		free_extent_buffer(eb);
		return;
	}

	target = search;

	nritems = btrfs_header_nritems(node);
	nr = slot;

	while (1) {
		if (path->reada == READA_BACK) {
			if (nr == 0)
				break;
			nr--;
		} else if (path->reada == READA_FORWARD) {
			nr++;
			if (nr >= nritems)
				break;
		}
		if (path->reada == READA_BACK && objectid) {
			btrfs_node_key(node, &disk_key, nr);
			if (btrfs_disk_key_objectid(&disk_key) != objectid)
				break;
		}
		search = btrfs_node_blockptr(node, nr);
		if ((search <= target && target - search <= 65536) ||
		    (search > target && search - target <= 65536)) {
			btrfs_readahead_node_child(node, nr);
			nread += blocksize;
		}
		nscan++;
		if ((nread > 65536 || nscan > 32))
			break;
	}
}