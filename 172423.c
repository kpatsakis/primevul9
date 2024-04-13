int scrub_enumerate_chunks(struct scrub_ctx *sctx,
			   struct btrfs_device *scrub_dev, u64 start, u64 end)
{
	struct btrfs_dev_extent *dev_extent = NULL;
	struct btrfs_path *path;
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	struct btrfs_root *root = fs_info->dev_root;
	u64 length;
	u64 chunk_offset;
	int ret = 0;
	int ro_set;
	int slot;
	struct extent_buffer *l;
	struct btrfs_key key;
	struct btrfs_key found_key;
	struct btrfs_block_group_cache *cache;
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	path->reada = READA_FORWARD;
	path->search_commit_root = 1;
	path->skip_locking = 1;

	key.objectid = scrub_dev->devid;
	key.offset = 0ull;
	key.type = BTRFS_DEV_EXTENT_KEY;

	while (1) {
		ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
		if (ret < 0)
			break;
		if (ret > 0) {
			if (path->slots[0] >=
			    btrfs_header_nritems(path->nodes[0])) {
				ret = btrfs_next_leaf(root, path);
				if (ret < 0)
					break;
				if (ret > 0) {
					ret = 0;
					break;
				}
			} else {
				ret = 0;
			}
		}

		l = path->nodes[0];
		slot = path->slots[0];

		btrfs_item_key_to_cpu(l, &found_key, slot);

		if (found_key.objectid != scrub_dev->devid)
			break;

		if (found_key.type != BTRFS_DEV_EXTENT_KEY)
			break;

		if (found_key.offset >= end)
			break;

		if (found_key.offset < key.offset)
			break;

		dev_extent = btrfs_item_ptr(l, slot, struct btrfs_dev_extent);
		length = btrfs_dev_extent_length(l, dev_extent);

		if (found_key.offset + length <= start)
			goto skip;

		chunk_offset = btrfs_dev_extent_chunk_offset(l, dev_extent);

		/*
		 * get a reference on the corresponding block group to prevent
		 * the chunk from going away while we scrub it
		 */
		cache = btrfs_lookup_block_group(fs_info, chunk_offset);

		/* some chunks are removed but not committed to disk yet,
		 * continue scrubbing */
		if (!cache)
			goto skip;

		/*
		 * we need call btrfs_inc_block_group_ro() with scrubs_paused,
		 * to avoid deadlock caused by:
		 * btrfs_inc_block_group_ro()
		 * -> btrfs_wait_for_commit()
		 * -> btrfs_commit_transaction()
		 * -> btrfs_scrub_pause()
		 */
		scrub_pause_on(fs_info);
		ret = btrfs_inc_block_group_ro(cache);
		if (!ret && sctx->is_dev_replace) {
			/*
			 * If we are doing a device replace wait for any tasks
			 * that started delalloc right before we set the block
			 * group to RO mode, as they might have just allocated
			 * an extent from it or decided they could do a nocow
			 * write. And if any such tasks did that, wait for their
			 * ordered extents to complete and then commit the
			 * current transaction, so that we can later see the new
			 * extent items in the extent tree - the ordered extents
			 * create delayed data references (for cow writes) when
			 * they complete, which will be run and insert the
			 * corresponding extent items into the extent tree when
			 * we commit the transaction they used when running
			 * inode.c:btrfs_finish_ordered_io(). We later use
			 * the commit root of the extent tree to find extents
			 * to copy from the srcdev into the tgtdev, and we don't
			 * want to miss any new extents.
			 */
			btrfs_wait_block_group_reservations(cache);
			btrfs_wait_nocow_writers(cache);
			ret = btrfs_wait_ordered_roots(fs_info, U64_MAX,
						       cache->key.objectid,
						       cache->key.offset);
			if (ret > 0) {
				struct btrfs_trans_handle *trans;

				trans = btrfs_join_transaction(root);
				if (IS_ERR(trans))
					ret = PTR_ERR(trans);
				else
					ret = btrfs_commit_transaction(trans);
				if (ret) {
					scrub_pause_off(fs_info);
					btrfs_put_block_group(cache);
					break;
				}
			}
		}
		scrub_pause_off(fs_info);

		if (ret == 0) {
			ro_set = 1;
		} else if (ret == -ENOSPC) {
			/*
			 * btrfs_inc_block_group_ro return -ENOSPC when it
			 * failed in creating new chunk for metadata.
			 * It is not a problem for scrub/replace, because
			 * metadata are always cowed, and our scrub paused
			 * commit_transactions.
			 */
			ro_set = 0;
		} else {
			btrfs_warn(fs_info,
				   "failed setting block group ro: %d", ret);
			btrfs_put_block_group(cache);
			break;
		}

		down_write(&fs_info->dev_replace.rwsem);
		dev_replace->cursor_right = found_key.offset + length;
		dev_replace->cursor_left = found_key.offset;
		dev_replace->item_needs_writeback = 1;
		up_write(&dev_replace->rwsem);

		ret = scrub_chunk(sctx, scrub_dev, chunk_offset, length,
				  found_key.offset, cache);

		/*
		 * flush, submit all pending read and write bios, afterwards
		 * wait for them.
		 * Note that in the dev replace case, a read request causes
		 * write requests that are submitted in the read completion
		 * worker. Therefore in the current situation, it is required
		 * that all write requests are flushed, so that all read and
		 * write requests are really completed when bios_in_flight
		 * changes to 0.
		 */
		sctx->flush_all_writes = true;
		scrub_submit(sctx);
		mutex_lock(&sctx->wr_lock);
		scrub_wr_submit(sctx);
		mutex_unlock(&sctx->wr_lock);

		wait_event(sctx->list_wait,
			   atomic_read(&sctx->bios_in_flight) == 0);

		scrub_pause_on(fs_info);

		/*
		 * must be called before we decrease @scrub_paused.
		 * make sure we don't block transaction commit while
		 * we are waiting pending workers finished.
		 */
		wait_event(sctx->list_wait,
			   atomic_read(&sctx->workers_pending) == 0);
		sctx->flush_all_writes = false;

		scrub_pause_off(fs_info);

		down_write(&fs_info->dev_replace.rwsem);
		dev_replace->cursor_left = dev_replace->cursor_right;
		dev_replace->item_needs_writeback = 1;
		up_write(&fs_info->dev_replace.rwsem);

		if (ro_set)
			btrfs_dec_block_group_ro(cache);

		/*
		 * We might have prevented the cleaner kthread from deleting
		 * this block group if it was already unused because we raced
		 * and set it to RO mode first. So add it back to the unused
		 * list, otherwise it might not ever be deleted unless a manual
		 * balance is triggered or it becomes used and unused again.
		 */
		spin_lock(&cache->lock);
		if (!cache->removed && !cache->ro && cache->reserved == 0 &&
		    btrfs_block_group_used(&cache->item) == 0) {
			spin_unlock(&cache->lock);
			btrfs_mark_bg_unused(cache);
		} else {
			spin_unlock(&cache->lock);
		}

		btrfs_put_block_group(cache);
		if (ret)
			break;
		if (sctx->is_dev_replace &&
		    atomic64_read(&dev_replace->num_write_errors) > 0) {
			ret = -EIO;
			break;
		}
		if (sctx->stat.malloc_errors > 0) {
			ret = -ENOMEM;
			break;
		}
skip:
		key.offset = found_key.offset + length;
		btrfs_release_path(path);
	}

	btrfs_free_path(path);

	return ret;
}