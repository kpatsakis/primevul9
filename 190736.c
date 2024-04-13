static int get_data_block_dio_write(struct inode *inode, sector_t iblock,
			struct buffer_head *bh_result, int create)
{
	return __get_data_block(inode, iblock, bh_result, create,
				F2FS_GET_BLOCK_DIO, NULL,
				f2fs_rw_hint_to_seg_type(inode->i_write_hint),
				true);
}