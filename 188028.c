static inline void packet_increment_head(struct packet_ring_buffer *buff)
{
	buff->head = buff->head != buff->frame_max ? buff->head+1 : 0;
}