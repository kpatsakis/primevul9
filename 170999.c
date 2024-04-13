int ring_buffer_record_is_on(struct ring_buffer *buffer)
{
	return !atomic_read(&buffer->record_disabled);
}