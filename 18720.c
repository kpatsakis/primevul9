static void xemaclite_aligned_read(u32 *src_ptr, u8 *dest_ptr,
				   unsigned length)
{
	u16 *to_u16_ptr, *from_u16_ptr;
	u32 *from_u32_ptr;
	u32 align_buffer;

	from_u32_ptr = src_ptr;
	to_u16_ptr = (u16 *)dest_ptr;

	for (; length > 3; length -= 4) {
		/* Copy each word into the temporary buffer */
		align_buffer = *from_u32_ptr++;
		from_u16_ptr = (u16 *)&align_buffer;

		/* Read data from source */
		*to_u16_ptr++ = *from_u16_ptr++;
		*to_u16_ptr++ = *from_u16_ptr++;
	}

	if (length) {
		u8 *to_u8_ptr, *from_u8_ptr;

		/* Set up to read the remaining data */
		to_u8_ptr = (u8 *)to_u16_ptr;
		align_buffer = *from_u32_ptr++;
		from_u8_ptr = (u8 *)&align_buffer;

		/* Read the remaining data */
		for (; length > 0; length--)
			*to_u8_ptr = *from_u8_ptr;
	}
}