int btrfs_read_sys_array(struct btrfs_fs_info *fs_info)
{
	struct btrfs_root *root = fs_info->tree_root;
	struct btrfs_super_block *super_copy = fs_info->super_copy;
	struct extent_buffer *sb;
	struct btrfs_disk_key *disk_key;
	struct btrfs_chunk *chunk;
	u8 *array_ptr;
	unsigned long sb_array_offset;
	int ret = 0;
	u32 num_stripes;
	u32 array_size;
	u32 len = 0;
	u32 cur_offset;
	u64 type;
	struct btrfs_key key;

	ASSERT(BTRFS_SUPER_INFO_SIZE <= fs_info->nodesize);
	/*
	 * This will create extent buffer of nodesize, superblock size is
	 * fixed to BTRFS_SUPER_INFO_SIZE. If nodesize > sb size, this will
	 * overallocate but we can keep it as-is, only the first page is used.
	 */
	sb = btrfs_find_create_tree_block(fs_info, BTRFS_SUPER_INFO_OFFSET);
	if (IS_ERR(sb))
		return PTR_ERR(sb);
	set_extent_buffer_uptodate(sb);
	btrfs_set_buffer_lockdep_class(root->root_key.objectid, sb, 0);
	/*
	 * The sb extent buffer is artificial and just used to read the system array.
	 * set_extent_buffer_uptodate() call does not properly mark all it's
	 * pages up-to-date when the page is larger: extent does not cover the
	 * whole page and consequently check_page_uptodate does not find all
	 * the page's extents up-to-date (the hole beyond sb),
	 * write_extent_buffer then triggers a WARN_ON.
	 *
	 * Regular short extents go through mark_extent_buffer_dirty/writeback cycle,
	 * but sb spans only this function. Add an explicit SetPageUptodate call
	 * to silence the warning eg. on PowerPC 64.
	 */
	if (PAGE_SIZE > BTRFS_SUPER_INFO_SIZE)
		SetPageUptodate(sb->pages[0]);

	write_extent_buffer(sb, super_copy, 0, BTRFS_SUPER_INFO_SIZE);
	array_size = btrfs_super_sys_array_size(super_copy);

	array_ptr = super_copy->sys_chunk_array;
	sb_array_offset = offsetof(struct btrfs_super_block, sys_chunk_array);
	cur_offset = 0;

	while (cur_offset < array_size) {
		disk_key = (struct btrfs_disk_key *)array_ptr;
		len = sizeof(*disk_key);
		if (cur_offset + len > array_size)
			goto out_short_read;

		btrfs_disk_key_to_cpu(&key, disk_key);

		array_ptr += len;
		sb_array_offset += len;
		cur_offset += len;

		if (key.type == BTRFS_CHUNK_ITEM_KEY) {
			chunk = (struct btrfs_chunk *)sb_array_offset;
			/*
			 * At least one btrfs_chunk with one stripe must be
			 * present, exact stripe count check comes afterwards
			 */
			len = btrfs_chunk_item_size(1);
			if (cur_offset + len > array_size)
				goto out_short_read;

			num_stripes = btrfs_chunk_num_stripes(sb, chunk);
			if (!num_stripes) {
				btrfs_err(fs_info,
					"invalid number of stripes %u in sys_array at offset %u",
					num_stripes, cur_offset);
				ret = -EIO;
				break;
			}

			type = btrfs_chunk_type(sb, chunk);
			if ((type & BTRFS_BLOCK_GROUP_SYSTEM) == 0) {
				btrfs_err(fs_info,
			    "invalid chunk type %llu in sys_array at offset %u",
					type, cur_offset);
				ret = -EIO;
				break;
			}

			len = btrfs_chunk_item_size(num_stripes);
			if (cur_offset + len > array_size)
				goto out_short_read;

			ret = read_one_chunk(fs_info, &key, sb, chunk);
			if (ret)
				break;
		} else {
			btrfs_err(fs_info,
			    "unexpected item type %u in sys_array at offset %u",
				  (u32)key.type, cur_offset);
			ret = -EIO;
			break;
		}
		array_ptr += len;
		sb_array_offset += len;
		cur_offset += len;
	}
	clear_extent_buffer_uptodate(sb);
	free_extent_buffer_stale(sb);
	return ret;

out_short_read:
	btrfs_err(fs_info, "sys_array too short to read %u bytes at offset %u",
			len, cur_offset);
	clear_extent_buffer_uptodate(sb);
	free_extent_buffer_stale(sb);
	return -EIO;
}