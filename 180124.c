ves_icall_System_Buffer_BlockCopyInternal (MonoArray *src, gint32 src_offset, MonoArray *dest, gint32 dest_offset, gint32 count) 
{
	guint8 *src_buf, *dest_buf;

	MONO_ARCH_SAVE_REGS;

	/* watch out for integer overflow */
	if ((src_offset > mono_array_get_byte_length (src) - count) || (dest_offset > mono_array_get_byte_length (dest) - count))
		return FALSE;

	src_buf = (guint8 *)src->vector + src_offset;
	dest_buf = (guint8 *)dest->vector + dest_offset;

	if (src != dest)
		memcpy (dest_buf, src_buf, count);
	else
		memmove (dest_buf, src_buf, count); /* Source and dest are the same array */

	return TRUE;
}