static unsigned rb_calculate_event_length(unsigned length)
{
	struct ring_buffer_event event; /* Used only for sizeof array */

	/* zero length can cause confusions */
	if (!length)
		length++;

	if (length > RB_MAX_SMALL_DATA || RB_FORCE_8BYTE_ALIGNMENT)
		length += sizeof(event.array[0]);

	length += RB_EVNT_HDR_SIZE;
	length = ALIGN(length, RB_ARCH_ALIGNMENT);

	/*
	 * In case the time delta is larger than the 27 bits for it
	 * in the header, we need to add a timestamp. If another
	 * event comes in when trying to discard this one to increase
	 * the length, then the timestamp will be added in the allocated
	 * space of this event. If length is bigger than the size needed
	 * for the TIME_EXTEND, then padding has to be used. The events
	 * length must be either RB_LEN_TIME_EXTEND, or greater than or equal
	 * to RB_LEN_TIME_EXTEND + 8, as 8 is the minimum size for padding.
	 * As length is a multiple of 4, we only need to worry if it
	 * is 12 (RB_LEN_TIME_EXTEND + 4).
	 */
	if (length == RB_LEN_TIME_EXTEND + RB_ALIGNMENT)
		length += RB_ALIGNMENT;

	return length;
}