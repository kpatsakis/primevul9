static void get_block_group_info(struct list_head *groups_list,
				 struct btrfs_ioctl_space_info *space)
{
	struct btrfs_block_group_cache *block_group;

	space->total_bytes = 0;
	space->used_bytes = 0;
	space->flags = 0;
	list_for_each_entry(block_group, groups_list, list) {
		space->flags = block_group->flags;
		space->total_bytes += block_group->key.offset;
		space->used_bytes +=
			btrfs_block_group_used(&block_group->item);
	}
}