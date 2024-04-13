static int f2fs_write_data_pages(struct address_space *mapping,
			    struct writeback_control *wbc)
{
	struct inode *inode = mapping->host;

	return __f2fs_write_data_pages(mapping, wbc,
			F2FS_I(inode)->cp_task == current ?
			FS_CP_DATA_IO : FS_DATA_IO);
}