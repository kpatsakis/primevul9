static inline unsigned long rb_page_write(struct buffer_page *bpage)
{
	return local_read(&bpage->write) & RB_WRITE_MASK;
}