int uncompress_gzip(void* uncompressed, size_t *uncompressed_len,
		const void* compressed, size_t compressed_len)
{
	z_stream stream;
	memset(&stream, 0, sizeof stream);
	stream.total_in = compressed_len;
	stream.avail_in = compressed_len;
	stream.total_out = *uncompressed_len;
	stream.avail_out = *uncompressed_len;
	stream.next_in = (Bytef *) compressed;
	stream.next_out = (Bytef *) uncompressed;

	/* 15 window bits, and the +32 tells zlib to to detect if using gzip or zlib */
	if (Z_OK == inflateInit2(&stream, (15 + 32))
			&& Z_STREAM_END == inflate(&stream, Z_FINISH)) {
		*uncompressed_len = stream.total_out;
	} else {
		return SC_ERROR_INVALID_DATA;
	}
	inflateEnd(&stream);

	return SC_SUCCESS;
}