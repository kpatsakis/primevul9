static FLAC__StreamEncoderInitStatus init_file_internal_(
	FLAC__StreamEncoder *encoder,
	const char *filename,
	FLAC__StreamEncoderProgressCallback progress_callback,
	void *client_data,
	FLAC__bool is_ogg
)
{
	FILE *file;

	FLAC__ASSERT(0 != encoder);

	/*
	 * To make sure that our file does not go unclosed after an error, we
	 * have to do the same entrance checks here that are later performed
	 * in FLAC__stream_encoder_init_FILE() before the FILE* is assigned.
	 */
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return FLAC__STREAM_ENCODER_INIT_STATUS_ALREADY_INITIALIZED;

	file = filename? flac_fopen(filename, "w+b") : stdout;

	if(file == 0) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_IO_ERROR;
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}

	return init_FILE_internal_(encoder, file, progress_callback, client_data, is_ogg);
}