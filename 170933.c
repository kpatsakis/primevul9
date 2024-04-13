static inline unsigned rb_page_commit(struct buffer_page *bpage)
{
	return local_read(&bpage->page->commit);
}