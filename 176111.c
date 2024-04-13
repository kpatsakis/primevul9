adv_error adv_png_write_chunk(adv_fz* f, unsigned type, const unsigned char* data, unsigned size, unsigned* count)
{
	unsigned char v[4];
	unsigned crc;

	be_uint32_write(v, size);
	if (fzwrite(v, 4, 1, f) != 1) {
		error_set("Error writing the chunk size");
		return -1;
	}

	be_uint32_write(v, type);
	if (fzwrite(v, 4, 1, f) != 1) {
		error_set("Error writing the chunk type");
		return -1;
	}

	crc = crc32(0, v, 4);
	if (size > 0) {
		if (fzwrite(data, size, 1, f) != 1) {
			error_set("Error writing the chunk data");
			return -1;
		}

		crc = crc32(crc, data, size);
	}

	be_uint32_write(v, crc);
	if (fzwrite(v, 4, 1, f) != 1) {
		error_set("Error writing the chunk crc");
		return -1;
	}

	if (count)
		*count += 4 + 4 + size + 4;

	return 0;
}