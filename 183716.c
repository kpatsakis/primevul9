FLAC_API FLAC__StreamEncoderInitStatus FLAC__stream_encoder_init_ogg_stream(
	FLAC__StreamEncoder *encoder,
	FLAC__StreamEncoderReadCallback read_callback,
	FLAC__StreamEncoderWriteCallback write_callback,
	FLAC__StreamEncoderSeekCallback seek_callback,
	FLAC__StreamEncoderTellCallback tell_callback,
	FLAC__StreamEncoderMetadataCallback metadata_callback,
	void *client_data
)
{
	return init_stream_internal_(
		encoder,
		read_callback,
		write_callback,
		seek_callback,
		tell_callback,
		metadata_callback,
		client_data,
		/*is_ogg=*/true
	);
}