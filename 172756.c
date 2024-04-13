static int mpage_map_and_submit_extent(handle_t *handle,
				       struct mpage_da_data *mpd,
				       bool *give_up_on_write)
{
	struct inode *inode = mpd->inode;
	struct ext4_map_blocks *map = &mpd->map;
	int err;
	loff_t disksize;
	int progress = 0;
	ext4_io_end_t *io_end = mpd->io_submit.io_end;
	struct ext4_io_end_vec *io_end_vec = ext4_alloc_io_end_vec(io_end);

	io_end_vec->offset = ((loff_t)map->m_lblk) << inode->i_blkbits;
	do {
		err = mpage_map_one_extent(handle, mpd);
		if (err < 0) {
			struct super_block *sb = inode->i_sb;

			if (ext4_forced_shutdown(EXT4_SB(sb)) ||
			    EXT4_SB(sb)->s_mount_flags & EXT4_MF_FS_ABORTED)
				goto invalidate_dirty_pages;
			/*
			 * Let the uper layers retry transient errors.
			 * In the case of ENOSPC, if ext4_count_free_blocks()
			 * is non-zero, a commit should free up blocks.
			 */
			if ((err == -ENOMEM) ||
			    (err == -ENOSPC && ext4_count_free_clusters(sb))) {
				if (progress)
					goto update_disksize;
				return err;
			}
			ext4_msg(sb, KERN_CRIT,
				 "Delayed block allocation failed for "
				 "inode %lu at logical offset %llu with"
				 " max blocks %u with error %d",
				 inode->i_ino,
				 (unsigned long long)map->m_lblk,
				 (unsigned)map->m_len, -err);
			ext4_msg(sb, KERN_CRIT,
				 "This should not happen!! Data will "
				 "be lost\n");
			if (err == -ENOSPC)
				ext4_print_free_blocks(inode);
		invalidate_dirty_pages:
			*give_up_on_write = true;
			return err;
		}
		progress = 1;
		/*
		 * Update buffer state, submit mapped pages, and get us new
		 * extent to map
		 */
		err = mpage_map_and_submit_buffers(mpd);
		if (err < 0)
			goto update_disksize;
	} while (map->m_len);

update_disksize:
	/*
	 * Update on-disk size after IO is submitted.  Races with
	 * truncate are avoided by checking i_size under i_data_sem.
	 */
	disksize = ((loff_t)mpd->first_page) << PAGE_SHIFT;
	if (disksize > EXT4_I(inode)->i_disksize) {
		int err2;
		loff_t i_size;

		down_write(&EXT4_I(inode)->i_data_sem);
		i_size = i_size_read(inode);
		if (disksize > i_size)
			disksize = i_size;
		if (disksize > EXT4_I(inode)->i_disksize)
			EXT4_I(inode)->i_disksize = disksize;
		up_write(&EXT4_I(inode)->i_data_sem);
		err2 = ext4_mark_inode_dirty(handle, inode);
		if (err2)
			ext4_error(inode->i_sb,
				   "Failed to mark inode %lu dirty",
				   inode->i_ino);
		if (!err)
			err = err2;
	}
	return err;
}