static noinline_for_stack int scrub_stripe(struct scrub_ctx *sctx,
					   struct map_lookup *map,
					   struct btrfs_device *scrub_dev,
					   int num, u64 base, u64 length)
{
	struct btrfs_path *path, *ppath;
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	struct btrfs_root *root = fs_info->extent_root;
	struct btrfs_root *csum_root = fs_info->csum_root;
	struct btrfs_extent_item *extent;
	struct blk_plug plug;
	u64 flags;
	int ret;
	int slot;
	u64 nstripes;
	struct extent_buffer *l;
	u64 physical;
	u64 logical;
	u64 logic_end;
	u64 physical_end;
	u64 generation;
	int mirror_num;
	struct reada_control *reada1;
	struct reada_control *reada2;
	struct btrfs_key key;
	struct btrfs_key key_end;
	u64 increment = map->stripe_len;
	u64 offset;
	u64 extent_logical;
	u64 extent_physical;
	u64 extent_len;
	u64 stripe_logical;
	u64 stripe_end;
	struct btrfs_device *extent_dev;
	int extent_mirror_num;
	int stop_loop = 0;

	physical = map->stripes[num].physical;
	offset = 0;
	nstripes = div64_u64(length, map->stripe_len);
	if (map->type & BTRFS_BLOCK_GROUP_RAID0) {
		offset = map->stripe_len * num;
		increment = map->stripe_len * map->num_stripes;
		mirror_num = 1;
	} else if (map->type & BTRFS_BLOCK_GROUP_RAID10) {
		int factor = map->num_stripes / map->sub_stripes;
		offset = map->stripe_len * (num / map->sub_stripes);
		increment = map->stripe_len * factor;
		mirror_num = num % map->sub_stripes + 1;
	} else if (map->type & BTRFS_BLOCK_GROUP_RAID1) {
		increment = map->stripe_len;
		mirror_num = num % map->num_stripes + 1;
	} else if (map->type & BTRFS_BLOCK_GROUP_DUP) {
		increment = map->stripe_len;
		mirror_num = num % map->num_stripes + 1;
	} else if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) {
		get_raid56_logic_offset(physical, num, map, &offset, NULL);
		increment = map->stripe_len * nr_data_stripes(map);
		mirror_num = 1;
	} else {
		increment = map->stripe_len;
		mirror_num = 1;
	}

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	ppath = btrfs_alloc_path();
	if (!ppath) {
		btrfs_free_path(path);
		return -ENOMEM;
	}

	/*
	 * work on commit root. The related disk blocks are static as
	 * long as COW is applied. This means, it is save to rewrite
	 * them to repair disk errors without any race conditions
	 */
	path->search_commit_root = 1;
	path->skip_locking = 1;

	ppath->search_commit_root = 1;
	ppath->skip_locking = 1;
	/*
	 * trigger the readahead for extent tree csum tree and wait for
	 * completion. During readahead, the scrub is officially paused
	 * to not hold off transaction commits
	 */
	logical = base + offset;
	physical_end = physical + nstripes * map->stripe_len;
	if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) {
		get_raid56_logic_offset(physical_end, num,
					map, &logic_end, NULL);
		logic_end += base;
	} else {
		logic_end = logical + increment * nstripes;
	}
	wait_event(sctx->list_wait,
		   atomic_read(&sctx->bios_in_flight) == 0);
	scrub_blocked_if_needed(fs_info);

	/* FIXME it might be better to start readahead at commit root */
	key.objectid = logical;
	key.type = BTRFS_EXTENT_ITEM_KEY;
	key.offset = (u64)0;
	key_end.objectid = logic_end;
	key_end.type = BTRFS_METADATA_ITEM_KEY;
	key_end.offset = (u64)-1;
	reada1 = btrfs_reada_add(root, &key, &key_end);

	key.objectid = BTRFS_EXTENT_CSUM_OBJECTID;
	key.type = BTRFS_EXTENT_CSUM_KEY;
	key.offset = logical;
	key_end.objectid = BTRFS_EXTENT_CSUM_OBJECTID;
	key_end.type = BTRFS_EXTENT_CSUM_KEY;
	key_end.offset = logic_end;
	reada2 = btrfs_reada_add(csum_root, &key, &key_end);

	if (!IS_ERR(reada1))
		btrfs_reada_wait(reada1);
	if (!IS_ERR(reada2))
		btrfs_reada_wait(reada2);


	/*
	 * collect all data csums for the stripe to avoid seeking during
	 * the scrub. This might currently (crc32) end up to be about 1MB
	 */
	blk_start_plug(&plug);

	/*
	 * now find all extents for each stripe and scrub them
	 */
	ret = 0;
	while (physical < physical_end) {
		/*
		 * canceled?
		 */
		if (atomic_read(&fs_info->scrub_cancel_req) ||
		    atomic_read(&sctx->cancel_req)) {
			ret = -ECANCELED;
			goto out;
		}
		/*
		 * check to see if we have to pause
		 */
		if (atomic_read(&fs_info->scrub_pause_req)) {
			/* push queued extents */
			sctx->flush_all_writes = true;
			scrub_submit(sctx);
			mutex_lock(&sctx->wr_lock);
			scrub_wr_submit(sctx);
			mutex_unlock(&sctx->wr_lock);
			wait_event(sctx->list_wait,
				   atomic_read(&sctx->bios_in_flight) == 0);
			sctx->flush_all_writes = false;
			scrub_blocked_if_needed(fs_info);
		}

		if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) {
			ret = get_raid56_logic_offset(physical, num, map,
						      &logical,
						      &stripe_logical);
			logical += base;
			if (ret) {
				/* it is parity strip */
				stripe_logical += base;
				stripe_end = stripe_logical + increment;
				ret = scrub_raid56_parity(sctx, map, scrub_dev,
							  ppath, stripe_logical,
							  stripe_end);
				if (ret)
					goto out;
				goto skip;
			}
		}

		if (btrfs_fs_incompat(fs_info, SKINNY_METADATA))
			key.type = BTRFS_METADATA_ITEM_KEY;
		else
			key.type = BTRFS_EXTENT_ITEM_KEY;
		key.objectid = logical;
		key.offset = (u64)-1;

		ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
		if (ret < 0)
			goto out;

		if (ret > 0) {
			ret = btrfs_previous_extent_item(root, path, 0);
			if (ret < 0)
				goto out;
			if (ret > 0) {
				/* there's no smaller item, so stick with the
				 * larger one */
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

			if (key.objectid + bytes <= logical)
				goto next;

			if (key.objectid >= logical + map->stripe_len) {
				/* out of this device extent */
				if (key.objectid >= logic_end)
					stop_loop = 1;
				break;
			}

			extent = btrfs_item_ptr(l, slot,
						struct btrfs_extent_item);
			flags = btrfs_extent_flags(l, extent);
			generation = btrfs_extent_generation(l, extent);

			if ((flags & BTRFS_EXTENT_FLAG_TREE_BLOCK) &&
			    (key.objectid < logical ||
			     key.objectid + bytes >
			     logical + map->stripe_len)) {
				btrfs_err(fs_info,
					   "scrub: tree block %llu spanning stripes, ignored. logical=%llu",
				       key.objectid, logical);
				spin_lock(&sctx->stat_lock);
				sctx->stat.uncorrectable_errors++;
				spin_unlock(&sctx->stat_lock);
				goto next;
			}

again:
			extent_logical = key.objectid;
			extent_len = bytes;

			/*
			 * trim extent to this stripe
			 */
			if (extent_logical < logical) {
				extent_len -= logical - extent_logical;
				extent_logical = logical;
			}
			if (extent_logical + extent_len >
			    logical + map->stripe_len) {
				extent_len = logical + map->stripe_len -
					     extent_logical;
			}

			extent_physical = extent_logical - logical + physical;
			extent_dev = scrub_dev;
			extent_mirror_num = mirror_num;
			if (sctx->is_dev_replace)
				scrub_remap_extent(fs_info, extent_logical,
						   extent_len, &extent_physical,
						   &extent_dev,
						   &extent_mirror_num);

			ret = btrfs_lookup_csums_range(csum_root,
						       extent_logical,
						       extent_logical +
						       extent_len - 1,
						       &sctx->csum_list, 1);
			if (ret)
				goto out;

			ret = scrub_extent(sctx, map, extent_logical, extent_len,
					   extent_physical, extent_dev, flags,
					   generation, extent_mirror_num,
					   extent_logical - logical + physical);

			scrub_free_csums(sctx);

			if (ret)
				goto out;

			if (extent_logical + extent_len <
			    key.objectid + bytes) {
				if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) {
					/*
					 * loop until we find next data stripe
					 * or we have finished all stripes.
					 */
loop:
					physical += map->stripe_len;
					ret = get_raid56_logic_offset(physical,
							num, map, &logical,
							&stripe_logical);
					logical += base;

					if (ret && physical < physical_end) {
						stripe_logical += base;
						stripe_end = stripe_logical +
								increment;
						ret = scrub_raid56_parity(sctx,
							map, scrub_dev, ppath,
							stripe_logical,
							stripe_end);
						if (ret)
							goto out;
						goto loop;
					}
				} else {
					physical += map->stripe_len;
					logical += increment;
				}
				if (logical < key.objectid + bytes) {
					cond_resched();
					goto again;
				}

				if (physical >= physical_end) {
					stop_loop = 1;
					break;
				}
			}
next:
			path->slots[0]++;
		}
		btrfs_release_path(path);
skip:
		logical += increment;
		physical += map->stripe_len;
		spin_lock(&sctx->stat_lock);
		if (stop_loop)
			sctx->stat.last_physical = map->stripes[num].physical +
						   length;
		else
			sctx->stat.last_physical = physical;
		spin_unlock(&sctx->stat_lock);
		if (stop_loop)
			break;
	}
out:
	/* push queued extents */
	scrub_submit(sctx);
	mutex_lock(&sctx->wr_lock);
	scrub_wr_submit(sctx);
	mutex_unlock(&sctx->wr_lock);

	blk_finish_plug(&plug);
	btrfs_free_path(path);
	btrfs_free_path(ppath);
	return ret < 0 ? ret : 0;
}