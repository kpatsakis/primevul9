static char *page_getlink(struct dentry * dentry, struct page **ppage)
{
	char *kaddr;
	struct page *page;
	struct address_space *mapping = dentry->d_inode->i_mapping;
	page = read_mapping_page(mapping, 0, NULL);
	if (IS_ERR(page))
		return (char*)page;
	*ppage = page;
	kaddr = kmap(page);
	nd_terminate_link(kaddr, dentry->d_inode->i_size, PAGE_SIZE - 1);
	return kaddr;
}