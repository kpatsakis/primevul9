static noinline_for_stack int scrub_raid56_parity(struct scrub_ctx *sctx,
						  struct map_lookup *map,
						  struct btrfs_device *sdev,
						  struct btrfs_path *path,
						  u64 logic_start,
						  u64 logic_end)
{
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	struct btrfs_root *root = fs_info->extent_root;
	struct btrfs_root *csum_root = fs_info->csum_root;
	struct btrfs_extent_item *extent;
	struct btrfs_bio *bbio = NULL;
	u64 flags;
	int ret;
	int slot;
	struct extent_buffer *l;
	struct btrfs_key key;
	u64 generation;
	u64 extent_logical;
	u64 extent_physical;
	u64 extent_len;
	u64 mapped_length;
	struct btrfs_device *extent_dev;
	struct scrub_parity *sparity;
	int nsectors;
	int bitmap_len;
	int extent_mirror_num;
	int stop_loop = 0;

	nsectors = div_u64(map->stripe_len, fs_info->sectorsize);
	bitmap_len = scrub_calc_parity_bitmap_len(nsectors);
	sparity = kzalloc(sizeof(struct scrub_parity) + 2 * bitmap_len,
			  GFP_NOFS);
	if (!sparity) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.malloc_errors++;
		spin_unlock(&sctx->stat_lock);
		return -ENOMEM;
	}

	sparity->stripe_len = map->stripe_len;
	sparity->nsectors = nsectors;
	sparity->sctx = sctx;
	sparity->scrub_dev = sdev;
	sparity->logic_start = logic_start;
	sparity->logic_end = logic_end;
	refcount_set(&sparity->refs, 1);
	INIT_LIST_HEAD(&sparity->spages);
	sparity->dbitmap = sparity->bitmap;
	sparity->ebitmap = (void *)sparity->bitmap + bitmap_len;

	ret = 0;
	while (logic_start < logic_end) {
		if (btrfs_fs_incompat(fs_info, SKINNY_METADATA))
			key.type = BTRFS_METADATA_ITEM_KEY;
		else
			key.type = BTRFS_EXTENT_ITEM_KEY;
		key.objectid = logic_start;
		key.offset = (u64)-1;

		ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
		if (ret < 0)
			goto out;

		if (ret > 0) {
			ret = btrfs_previous_extent_item(root, path, 0);
			if (ret < 0)
				goto out;
			if (ret > 0) {
				btrfs_release_path(path);
				ret = btrfs_search_slot(NULL, root, &key,
							path, 0, 0);
				if (ret < 0)
					goto out;
			}
		}

		stop_loop = 0;
		while (1) {
			u64 bytes;

			l = path->nodes[0];
			slot = path->slots[0];
			if (slot >= btrfs_header_nritems(l)) {
				ret = btrfs_next_leaf(root, path);
				if (ret == 0)
					continue;
				if (ret < 0)
					goto out;

				stop_loop = 1;
				break;
			}
			btrfs_item_key_to_cpu(l, &key, slot);

			if (key.type != BTRFS_EXTENT_ITEM_KEY &&
			    key.type != BTRFS_METADATA_ITEM_KEY)
				goto next;

			if (key.type == BTRFS_METADATA_ITEM_KEY)
				bytes = fs_info->nodesize;
			else
				bytes = key.offset;

			if (key.objectid + bytes <= logic_start)
				goto next;

			if (key.objectid >= logic_end) {
				stop_loop = 1;
				break;
			}

			while (key.objectid >= logic_start + map->stripe_len)
				logic_start += map->stripe_len;

			extent = btrfs_item_ptr(l, slot,
						struct btrfs_extent_item);
			flags = btrfs_extent_flags(l, extent);
			generation = btrfs_extent_generation(l, extent);

			if ((flags & BTRFS_EXTENT_FLAG_TREE_BLOCK) &&
			    (key.objectid < logic_start ||
			     key.objectid + bytes >
			     logic_start + map->stripe_len)) {
				btrfs_err(fs_info,
					  "scrub: tree block %llu spanning stripes, ignored. logical=%llu",
					  key.objectid, logic_start);
				spin_lock(&sctx->stat_lock);
				sctx->stat.uncorrectable_errors++;
				spin_unlock(&sctx->stat_lock);
				goto next;
			}
again:
			extent_logical = key.objectid;
			extent_len = bytes;

			if (extent_logical < logic_start) {
				extent_len -= logic_start - extent_logical;
				extent_logical = logic_start;
			}

			if (extent_logical + extent_len >
			    logic_start + map->stripe_len)
				extent_len = logic_start + map->stripe_len -
					     extent_logical;

			scrub_parity_mark_sectors_data(sparity, extent_logical,
						       extent_len);

			mapped_length = extent_len;
			bbio = NULL;
			ret = btrfs_map_block(fs_info, BTRFS_MAP_READ,
					extent_logical, &mapped_length, &bbio,
					0);
			if (!ret) {
				if (!bbio || mapped_length < extent_len)
					ret = -EIO;
			}
			if (ret) {
				btrfs_put_bbio(bbio);
				goto out;
			}
			extent_physical = bbio->stripes[0].physical;
			extent_mirror_num = bbio->mirror_num;
			extent_dev = bbio->stripes[0].dev;
			btrfs_put_bbio(bbio);

			ret = btrfs_lookup_csums_range(csum_root,
						extent_logical,
						extent_logical + extent_len - 1,
						&sctx->csum_list, 1);
			if (ret)
				goto out;

			ret = scrub_extent_for_parity(sparity, extent_logical,
						      extent_len,
						      extent_physical,
						      extent_dev, flags,
						      generation,
						      extent_mirror_num);

			scrub_free_csums(sctx);

			if (ret)
				goto out;

			if (extent_logical + extent_len <
			    key.objectid + bytes) {
				logic_start += map->stripe_len;

				if (logic_start >= logic_end) {
					stop_loop = 1;
					break;
				}

				if (logic_start < key.objectid + bytes) {
					cond_resched();
					goto again;
				}
			}
next:
			path->slots[0]++;
		}

		btrfs_release_path(path);

		if (stop_loop)
			break;

		logic_start += map->stripe_len;
	}
out:
	if (ret < 0)
		scrub_parity_mark_sectors_error(sparity, logic_start,
						logic_end - logic_start);
	scrub_parity_put(sparity);
	scrub_submit(sctx);
	mutex_lock(&sctx->wr_lock);
	scrub_wr_submit(sctx);
	mutex_unlock(&sctx->wr_lock);

	btrfs_release_path(path);
	return ret < 0 ? ret : 0;
}