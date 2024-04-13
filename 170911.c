void ring_buffer_set_clock(struct ring_buffer *buffer,
			   u64 (*clock)(void))
{
	buffer->clock = clock;
}