static int __btrfs_map_block(struct btrfs_fs_info *fs_info,
			     enum btrfs_map_op op,
			     u64 logical, u64 *length,
			     struct btrfs_bio **bbio_ret,
			     int mirror_num, int need_raid_map)
{
	struct extent_map *em;
	struct map_lookup *map;
	u64 offset;
	u64 stripe_offset;
	u64 stripe_nr;
	u64 stripe_len;
	u32 stripe_index;
	int i;
	int ret = 0;
	int num_stripes;
	int max_errors = 0;
	int tgtdev_indexes = 0;
	struct btrfs_bio *bbio = NULL;
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;
	int dev_replace_is_ongoing = 0;
	int num_alloc_stripes;
	int patch_the_first_stripe_for_dev_replace = 0;
	u64 physical_to_patch_in_first_stripe = 0;
	u64 raid56_full_stripe_start = (u64)-1;

	if (op == BTRFS_MAP_DISCARD)
		return __btrfs_map_block_for_discard(fs_info, logical,
						     *length, bbio_ret);

	em = btrfs_get_chunk_map(fs_info, logical, *length);
	if (IS_ERR(em))
		return PTR_ERR(em);

	map = em->map_lookup;
	offset = logical - em->start;

	stripe_len = map->stripe_len;
	stripe_nr = offset;
	/*
	 * stripe_nr counts the total number of stripes we have to stride
	 * to get to this block
	 */
	stripe_nr = div64_u64(stripe_nr, stripe_len);

	stripe_offset = stripe_nr * stripe_len;
	if (offset < stripe_offset) {
		btrfs_crit(fs_info,
			   "stripe math has gone wrong, stripe_offset=%llu, offset=%llu, start=%llu, logical=%llu, stripe_len=%llu",
			   stripe_offset, offset, em->start, logical,
			   stripe_len);
		free_extent_map(em);
		return -EINVAL;
	}

	/* stripe_offset is the offset of this block in its stripe*/
	stripe_offset = offset - stripe_offset;

	/* if we're here for raid56, we need to know the stripe aligned start */
	if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) {
		unsigned long full_stripe_len = stripe_len * nr_data_stripes(map);
		raid56_full_stripe_start = offset;

		/* allow a write of a full stripe, but make sure we don't
		 * allow straddling of stripes
		 */
		raid56_full_stripe_start = div64_u64(raid56_full_stripe_start,
				full_stripe_len);
		raid56_full_stripe_start *= full_stripe_len;
	}

	if (map->type & BTRFS_BLOCK_GROUP_PROFILE_MASK) {
		u64 max_len;
		/* For writes to RAID[56], allow a full stripeset across all disks.
		   For other RAID types and for RAID[56] reads, just allow a single
		   stripe (on a single disk). */
		if ((map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) &&
		    (op == BTRFS_MAP_WRITE)) {
			max_len = stripe_len * nr_data_stripes(map) -
				(offset - raid56_full_stripe_start);
		} else {
			/* we limit the length of each bio to what fits in a stripe */
			max_len = stripe_len - stripe_offset;
		}
		*length = min_t(u64, em->len - offset, max_len);
	} else {
		*length = em->len - offset;
	}

	/*
	 * This is for when we're called from btrfs_bio_fits_in_stripe and all
	 * it cares about is the length
	 */
	if (!bbio_ret)
		goto out;

	down_read(&dev_replace->rwsem);
	dev_replace_is_ongoing = btrfs_dev_replace_is_ongoing(dev_replace);
	/*
	 * Hold the semaphore for read during the whole operation, write is
	 * requested at commit time but must wait.
	 */
	if (!dev_replace_is_ongoing)
		up_read(&dev_replace->rwsem);

	if (dev_replace_is_ongoing && mirror_num == map->num_stripes + 1 &&
	    !need_full_stripe(op) && dev_replace->tgtdev != NULL) {
		ret = get_extra_mirror_from_replace(fs_info, logical, *length,
						    dev_replace->srcdev->devid,
						    &mirror_num,
					    &physical_to_patch_in_first_stripe);
		if (ret)
			goto out;
		else
			patch_the_first_stripe_for_dev_replace = 1;
	} else if (mirror_num > map->num_stripes) {
		mirror_num = 0;
	}

	num_stripes = 1;
	stripe_index = 0;
	if (map->type & BTRFS_BLOCK_GROUP_RAID0) {
		stripe_nr = div_u64_rem(stripe_nr, map->num_stripes,
				&stripe_index);
		if (!need_full_stripe(op))
			mirror_num = 1;
	} else if (map->type & BTRFS_BLOCK_GROUP_RAID1) {
		if (need_full_stripe(op))
			num_stripes = map->num_stripes;
		else if (mirror_num)
			stripe_index = mirror_num - 1;
		else {
			stripe_index = find_live_mirror(fs_info, map, 0,
					    dev_replace_is_ongoing);
			mirror_num = stripe_index + 1;
		}

	} else if (map->type & BTRFS_BLOCK_GROUP_DUP) {
		if (need_full_stripe(op)) {
			num_stripes = map->num_stripes;
		} else if (mirror_num) {
			stripe_index = mirror_num - 1;
		} else {
			mirror_num = 1;
		}

	} else if (map->type & BTRFS_BLOCK_GROUP_RAID10) {
		u32 factor = map->num_stripes / map->sub_stripes;

		stripe_nr = div_u64_rem(stripe_nr, factor, &stripe_index);
		stripe_index *= map->sub_stripes;

		if (need_full_stripe(op))
			num_stripes = map->sub_stripes;
		else if (mirror_num)
			stripe_index += mirror_num - 1;
		else {
			int old_stripe_index = stripe_index;
			stripe_index = find_live_mirror(fs_info, map,
					      stripe_index,
					      dev_replace_is_ongoing);
			mirror_num = stripe_index - old_stripe_index + 1;
		}

	} else if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) {
		if (need_raid_map && (need_full_stripe(op) || mirror_num > 1)) {
			/* push stripe_nr back to the start of the full stripe */
			stripe_nr = div64_u64(raid56_full_stripe_start,
					stripe_len * nr_data_stripes(map));

			/* RAID[56] write or recovery. Return all stripes */
			num_stripes = map->num_stripes;
			max_errors = nr_parity_stripes(map);

			*length = map->stripe_len;
			stripe_index = 0;
			stripe_offset = 0;
		} else {
			/*
			 * Mirror #0 or #1 means the original data block.
			 * Mirror #2 is RAID5 parity block.
			 * Mirror #3 is RAID6 Q block.
			 */
			stripe_nr = div_u64_rem(stripe_nr,
					nr_data_stripes(map), &stripe_index);
			if (mirror_num > 1)
				stripe_index = nr_data_stripes(map) +
						mirror_num - 2;

			/* We distribute the parity blocks across stripes */
			div_u64_rem(stripe_nr + stripe_index, map->num_stripes,
					&stripe_index);
			if (!need_full_stripe(op) && mirror_num <= 1)
				mirror_num = 1;
		}
	} else {
		/*
		 * after this, stripe_nr is the number of stripes on this
		 * device we have to walk to find the data, and stripe_index is
		 * the number of our device in the stripe array
		 */
		stripe_nr = div_u64_rem(stripe_nr, map->num_stripes,
				&stripe_index);
		mirror_num = stripe_index + 1;
	}
	if (stripe_index >= map->num_stripes) {
		btrfs_crit(fs_info,
			   "stripe index math went horribly wrong, got stripe_index=%u, num_stripes=%u",
			   stripe_index, map->num_stripes);
		ret = -EINVAL;
		goto out;
	}

	num_alloc_stripes = num_stripes;
	if (dev_replace_is_ongoing && dev_replace->tgtdev != NULL) {
		if (op == BTRFS_MAP_WRITE)
			num_alloc_stripes <<= 1;
		if (op == BTRFS_MAP_GET_READ_MIRRORS)
			num_alloc_stripes++;
		tgtdev_indexes = num_stripes;
	}

	bbio = alloc_btrfs_bio(num_alloc_stripes, tgtdev_indexes);
	if (!bbio) {
		ret = -ENOMEM;
		goto out;
	}
	if (dev_replace_is_ongoing && dev_replace->tgtdev != NULL)
		bbio->tgtdev_map = (int *)(bbio->stripes + num_alloc_stripes);

	/* build raid_map */
	if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK && need_raid_map &&
	    (need_full_stripe(op) || mirror_num > 1)) {
		u64 tmp;
		unsigned rot;

		bbio->raid_map = (u64 *)((void *)bbio->stripes +
				 sizeof(struct btrfs_bio_stripe) *
				 num_alloc_stripes +
				 sizeof(int) * tgtdev_indexes);

		/* Work out the disk rotation on this stripe-set */
		div_u64_rem(stripe_nr, num_stripes, &rot);

		/* Fill in the logical address of each stripe */
		tmp = stripe_nr * nr_data_stripes(map);
		for (i = 0; i < nr_data_stripes(map); i++)
			bbio->raid_map[(i+rot) % num_stripes] =
				em->start + (tmp + i) * map->stripe_len;

		bbio->raid_map[(i+rot) % map->num_stripes] = RAID5_P_STRIPE;
		if (map->type & BTRFS_BLOCK_GROUP_RAID6)
			bbio->raid_map[(i+rot+1) % num_stripes] =
				RAID6_Q_STRIPE;
	}


	for (i = 0; i < num_stripes; i++) {
		bbio->stripes[i].physical =
			map->stripes[stripe_index].physical +
			stripe_offset +
			stripe_nr * map->stripe_len;
		bbio->stripes[i].dev =
			map->stripes[stripe_index].dev;
		stripe_index++;
	}

	if (need_full_stripe(op))
		max_errors = btrfs_chunk_max_errors(map);

	if (bbio->raid_map)
		sort_parity_stripes(bbio, num_stripes);

	if (dev_replace_is_ongoing && dev_replace->tgtdev != NULL &&
	    need_full_stripe(op)) {
		handle_ops_on_dev_replace(op, &bbio, dev_replace, &num_stripes,
					  &max_errors);
	}

	*bbio_ret = bbio;
	bbio->map_type = map->type;
	bbio->num_stripes = num_stripes;
	bbio->max_errors = max_errors;
	bbio->mirror_num = mirror_num;

	/*
	 * this is the case that REQ_READ && dev_replace_is_ongoing &&
	 * mirror_num == num_stripes + 1 && dev_replace target drive is
	 * available as a mirror
	 */
	if (patch_the_first_stripe_for_dev_replace && num_stripes > 0) {
		WARN_ON(num_stripes > 1);
		bbio->stripes[0].dev = dev_replace->tgtdev;
		bbio->stripes[0].physical = physical_to_patch_in_first_stripe;
		bbio->mirror_num = map->num_stripes + 1;
	}
out:
	if (dev_replace_is_ongoing) {
		lockdep_assert_held(&dev_replace->rwsem);
		/* Unlock and let waiting writers proceed */
		up_read(&dev_replace->rwsem);
	}
	free_extent_map(em);
	return ret;
}