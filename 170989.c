static void free_buffer_page(struct buffer_page *bpage)
{
	free_page((unsigned long)bpage->page);
	kfree(bpage);
}