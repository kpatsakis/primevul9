FLAC_API FLAC__StreamEncoderInitStatus FLAC__stream_encoder_init_ogg_file(
	FLAC__StreamEncoder *encoder,
	const char *filename,
	FLAC__StreamEncoderProgressCallback progress_callback,
	void *client_data
)
{
	return init_file_internal_(encoder, filename, progress_callback, client_data, /*is_ogg=*/true);
}