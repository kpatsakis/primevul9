static void btrfs_release_disk_super(struct page *page)
{
	kunmap(page);
	put_page(page);
}