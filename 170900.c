static inline u64 rb_time_stamp(struct ring_buffer *buffer)
{
	/* shift to debug/test normalization and TIME_EXTENTS */
	return buffer->clock() << DEBUG_SHIFT;
}