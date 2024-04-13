void ring_buffer_record_off(struct ring_buffer *buffer)
{
	unsigned int rd;
	unsigned int new_rd;

	do {
		rd = atomic_read(&buffer->record_disabled);
		new_rd = rd | RB_BUFFER_OFF;
	} while (atomic_cmpxchg(&buffer->record_disabled, rd, new_rd) != rd);
}