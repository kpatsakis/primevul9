static FLAC__StreamEncoderInitStatus init_FILE_internal_(
	FLAC__StreamEncoder *encoder,
	FILE *file,
	FLAC__StreamEncoderProgressCallback progress_callback,
	void *client_data,
	FLAC__bool is_ogg
)
{
	FLAC__StreamEncoderInitStatus init_status;

	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != file);

	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return FLAC__STREAM_ENCODER_INIT_STATUS_ALREADY_INITIALIZED;

	/* double protection */
	if(file == 0) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_IO_ERROR;
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}

	/*
	 * To make sure that our file does not go unclosed after an error, we
	 * must assign the FILE pointer before any further error can occur in
	 * this routine.
	 */
	if(file == stdout)
		file = get_binary_stdout_(); /* just to be safe */

#ifdef _WIN32
	/*
	 * Windows can suffer quite badly from disk fragmentation. This can be
	 * reduced significantly by setting the output buffer size to be 10MB.
	 */
	if(GetFileType((HANDLE)_get_osfhandle(_fileno(file))) == FILE_TYPE_DISK)
		setvbuf(file, NULL, _IOFBF, 10*1024*1024);
#endif
	encoder->private_->file = file;

	encoder->private_->progress_callback = progress_callback;
	encoder->private_->bytes_written = 0;
	encoder->private_->samples_written = 0;
	encoder->private_->frames_written = 0;

	init_status = init_stream_internal_(
		encoder,
		encoder->private_->file == stdout? 0 : is_ogg? file_read_callback_ : 0,
		file_write_callback_,
		encoder->private_->file == stdout? 0 : file_seek_callback_,
		encoder->private_->file == stdout? 0 : file_tell_callback_,
		/*metadata_callback=*/0,
		client_data,
		is_ogg
	);
	if(init_status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
		/* the above function sets the state for us in case of an error */
		return init_status;
	}

	{
		uint32_t blocksize = FLAC__stream_encoder_get_blocksize(encoder);

		FLAC__ASSERT(blocksize != 0);
		encoder->private_->total_frames_estimate = (uint32_t)((FLAC__stream_encoder_get_total_samples_estimate(encoder) + blocksize - 1) / blocksize);
	}

	return init_status;
}