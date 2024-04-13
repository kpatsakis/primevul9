void ring_buffer_record_disable(struct ring_buffer *buffer)
{
	atomic_inc(&buffer->record_disabled);
}