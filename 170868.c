rb_iter_head_event(struct ring_buffer_iter *iter)
{
	return __rb_page_index(iter->head_page, iter->head);
}