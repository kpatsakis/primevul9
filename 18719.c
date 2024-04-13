static void xemaclite_aligned_write(void *src_ptr, u32 *dest_ptr,
				    unsigned length)
{
	u32 align_buffer;
	u32 *to_u32_ptr;
	u16 *from_u16_ptr, *to_u16_ptr;

	to_u32_ptr = dest_ptr;
	from_u16_ptr = src_ptr;
	align_buffer = 0;

	for (; length > 3; length -= 4) {
		to_u16_ptr = (u16 *)&align_buffer;
		*to_u16_ptr++ = *from_u16_ptr++;
		*to_u16_ptr++ = *from_u16_ptr++;

		/* This barrier resolves occasional issues seen around
		 * cases where the data is not properly flushed out
		 * from the processor store buffers to the destination
		 * memory locations.
		 */
		wmb();

		/* Output a word */
		*to_u32_ptr++ = align_buffer;
	}
	if (length) {
		u8 *from_u8_ptr, *to_u8_ptr;

		/* Set up to output the remaining data */
		align_buffer = 0;
		to_u8_ptr = (u8 *)&align_buffer;
		from_u8_ptr = (u8 *)from_u16_ptr;

		/* Output the remaining data */
		for (; length > 0; length--)
			*to_u8_ptr++ = *from_u8_ptr++;

		/* This barrier resolves occasional issues seen around
		 * cases where the data is not properly flushed out
		 * from the processor store buffers to the destination
		 * memory locations.
		 */
		wmb();
		*to_u32_ptr = align_buffer;
	}
}