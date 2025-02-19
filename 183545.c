static int compress_row(unsigned char *row, int length)
{
	int rle_type = 0;
	int compressed_length = 0;
	int pixel = 0, compressed_run = 0, rle_compression = 0;
	unsigned char *uncompressed_row = NULL, *uncompressed_rowp = NULL, *uncompressed_start = NULL;

	uncompressed_row = (unsigned char *) gdMalloc(length);
	if (!uncompressed_row) {
		return -1;
	}

	memcpy(uncompressed_row, row, length);
	uncompressed_start = uncompressed_rowp = uncompressed_row;

	for (pixel = 0; pixel < length; pixel++) {
		if (compressed_run == 0) {
			uncompressed_row = uncompressed_rowp;
			compressed_run++;
			uncompressed_rowp++;
			rle_type = BMP_RLE_TYPE_RAW;
			continue;
		}

		if (compressed_run == 1) {
			/* Compare next byte */
			if (memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) == 0) {
				rle_type = BMP_RLE_TYPE_RLE;
			}
		}

		if (rle_type == BMP_RLE_TYPE_RLE) {
			if (compressed_run >= 128 || memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) != 0) {
				/* more than what we can store in a single run or run is over due to non match, force write */
				rle_compression = build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
				row += rle_compression;
				compressed_length += rle_compression;
				compressed_run = 0;
				pixel--;
			} else {
				compressed_run++;
				uncompressed_rowp++;
			}
		} else {
			if (compressed_run >= 128 || memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) == 0) {
				/* more than what we can store in a single run or run is over due to match, force write */
				rle_compression = build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
				row += rle_compression;
				compressed_length += rle_compression;
				compressed_run = 0;
				pixel--;
			} else {
				/* add this pixel to the row */
				compressed_run++;
				uncompressed_rowp++;
			}

		}
	}

	if (compressed_run) {
		compressed_length += build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
	}

	gdFree(uncompressed_start);

	return compressed_length;
}