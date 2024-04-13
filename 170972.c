static inline unsigned long rb_page_entries(struct buffer_page *bpage)
{
	return local_read(&bpage->entries) & RB_WRITE_MASK;
}