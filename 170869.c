static inline void *__rb_page_index(struct buffer_page *bpage, unsigned index)
{
	return bpage->page->data + index;
}