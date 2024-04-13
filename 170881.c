void ring_buffer_record_enable(struct ring_buffer *buffer)
{
	atomic_dec(&buffer->record_disabled);
}