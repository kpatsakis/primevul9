static inline unsigned rb_page_size(struct buffer_page *bpage)
{
	return rb_page_commit(bpage);
}