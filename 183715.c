FLAC_API FLAC__StreamEncoderInitStatus FLAC__stream_encoder_init_FILE(
	FLAC__StreamEncoder *encoder,
	FILE *file,
	FLAC__StreamEncoderProgressCallback progress_callback,
	void *client_data
)
{
	return init_FILE_internal_(encoder, file, progress_callback, client_data, /*is_ogg=*/false);
}