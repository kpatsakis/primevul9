int f2fs_preallocate_blocks(struct kiocb *iocb, struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct f2fs_map_blocks map;
	int flag;
	int err = 0;
	bool direct_io = iocb->ki_flags & IOCB_DIRECT;

	/* convert inline data for Direct I/O*/
	if (direct_io) {
		err = f2fs_convert_inline_inode(inode);
		if (err)
			return err;
	}

	if (direct_io && allow_outplace_dio(inode, iocb, from))
		return 0;

	if (is_inode_flag_set(inode, FI_NO_PREALLOC))
		return 0;

	map.m_lblk = F2FS_BLK_ALIGN(iocb->ki_pos);
	map.m_len = F2FS_BYTES_TO_BLK(iocb->ki_pos + iov_iter_count(from));
	if (map.m_len > map.m_lblk)
		map.m_len -= map.m_lblk;
	else
		map.m_len = 0;

	map.m_next_pgofs = NULL;
	map.m_next_extent = NULL;
	map.m_seg_type = NO_CHECK_TYPE;
	map.m_may_create = true;

	if (direct_io) {
		map.m_seg_type = f2fs_rw_hint_to_seg_type(iocb->ki_hint);
		flag = f2fs_force_buffered_io(inode, iocb, from) ?
					F2FS_GET_BLOCK_PRE_AIO :
					F2FS_GET_BLOCK_PRE_DIO;
		goto map_blocks;
	}
	if (iocb->ki_pos + iov_iter_count(from) > MAX_INLINE_DATA(inode)) {
		err = f2fs_convert_inline_inode(inode);
		if (err)
			return err;
	}
	if (f2fs_has_inline_data(inode))
		return err;

	flag = F2FS_GET_BLOCK_PRE_AIO;

map_blocks:
	err = f2fs_map_blocks(inode, &map, 1, flag);
	if (map.m_len > 0 && err == -ENOSPC) {
		if (!direct_io)
			set_inode_flag(inode, FI_NO_PREALLOC);
		err = 0;
	}
	return err;
}