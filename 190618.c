static int f2fs_write_data_page(struct page *page,
					struct writeback_control *wbc)
{
	return __write_data_page(page, NULL, NULL, NULL, wbc, FS_DATA_IO);
}