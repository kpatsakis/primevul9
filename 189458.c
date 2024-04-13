void png_compress_palette_delta(data_ptr& out_ptr, unsigned& out_size, const unsigned char* pal_ptr, unsigned pal_size, const unsigned char* prev_ptr, unsigned prev_size)
{
	unsigned i;
	unsigned char* dst_ptr;
	unsigned dst_size;

	dst_ptr = data_alloc(pal_size * 2);
	dst_size = 0;

	dst_ptr[dst_size++] = 0; /* replacement */

	i = 0;
	while (i<pal_size) {
		unsigned j;

		while (i < pal_size && (i < prev_size && prev_ptr[i] == pal_ptr[i] && prev_ptr[i+1] == pal_ptr[i+1] && prev_ptr[i+2] == pal_ptr[i+2]))
			i += 3;

		if (i == pal_size)
			break;

		j = i + 3;

		while (j < pal_size && (j >= prev_size || prev_ptr[j] != pal_ptr[j] || prev_ptr[j+1] != pal_ptr[j+1] || prev_ptr[j+2] != pal_ptr[j+2]))
			j += 3;

		dst_ptr[dst_size++] = i / 3; /* first index */
		dst_ptr[dst_size++] = (j / 3) - 1; /* last index */

		while (i < j) {
			dst_ptr[dst_size++] = pal_ptr[i++];
			dst_ptr[dst_size++] = pal_ptr[i++];
			dst_ptr[dst_size++] = pal_ptr[i++];
		}
	}

	if (dst_size == 1) {
		out_ptr = 0;
		out_size = 0;
		free(dst_ptr);
	} else {
		out_ptr = dst_ptr;
		out_size = dst_size;
	}
}