void ring_buffer_free_read_page(struct ring_buffer *buffer, void *data)
{
	free_page((unsigned long)data);
}