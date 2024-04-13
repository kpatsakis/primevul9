FLAC__StreamEncoderWriteStatus write_frame_(FLAC__StreamEncoder *encoder, const FLAC__byte buffer[], size_t bytes, uint32_t samples, FLAC__bool is_last_block)
{
	FLAC__StreamEncoderWriteStatus status;
	FLAC__uint64 output_position = 0;

#if FLAC__HAS_OGG == 0
	(void)is_last_block;
#endif

	/* FLAC__STREAM_ENCODER_TELL_STATUS_UNSUPPORTED just means we didn't get the offset; no error */
	if(encoder->private_->tell_callback && encoder->private_->tell_callback(encoder, &output_position, encoder->private_->client_data) == FLAC__STREAM_ENCODER_TELL_STATUS_ERROR) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_CLIENT_ERROR;
		return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
	}

	/*
	 * Watch for the STREAMINFO block and first SEEKTABLE block to go by and store their offsets.
	 */
	if(samples == 0) {
		FLAC__MetadataType type = (buffer[0] & 0x7f);
		if(type == FLAC__METADATA_TYPE_STREAMINFO)
			encoder->protected_->streaminfo_offset = output_position;
		else if(type == FLAC__METADATA_TYPE_SEEKTABLE && encoder->protected_->seektable_offset == 0)
			encoder->protected_->seektable_offset = output_position;
	}

	/*
	 * Mark the current seek point if hit (if audio_offset == 0 that
	 * means we're still writing metadata and haven't hit the first
	 * frame yet)
	 */
	if(0 != encoder->private_->seek_table && encoder->protected_->audio_offset > 0 && encoder->private_->seek_table->num_points > 0) {
		const uint32_t blocksize = FLAC__stream_encoder_get_blocksize(encoder);
		const FLAC__uint64 frame_first_sample = encoder->private_->samples_written;
		const FLAC__uint64 frame_last_sample = frame_first_sample + (FLAC__uint64)blocksize - 1;
		FLAC__uint64 test_sample;
		uint32_t i;
		for(i = encoder->private_->first_seekpoint_to_check; i < encoder->private_->seek_table->num_points; i++) {
			test_sample = encoder->private_->seek_table->points[i].sample_number;
			if(test_sample > frame_last_sample) {
				break;
			}
			else if(test_sample >= frame_first_sample) {
				encoder->private_->seek_table->points[i].sample_number = frame_first_sample;
				encoder->private_->seek_table->points[i].stream_offset = output_position - encoder->protected_->audio_offset;
				encoder->private_->seek_table->points[i].frame_samples = blocksize;
				encoder->private_->first_seekpoint_to_check++;
				/* DO NOT: "break;" and here's why:
				 * The seektable template may contain more than one target
				 * sample for any given frame; we will keep looping, generating
				 * duplicate seekpoints for them, and we'll clean it up later,
				 * just before writing the seektable back to the metadata.
				 */
			}
			else {
				encoder->private_->first_seekpoint_to_check++;
			}
		}
	}

#if FLAC__HAS_OGG
	if(encoder->private_->is_ogg) {
		status = FLAC__ogg_encoder_aspect_write_callback_wrapper(
			&encoder->protected_->ogg_encoder_aspect,
			buffer,
			bytes,
			samples,
			encoder->private_->current_frame_number,
			is_last_block,
			(FLAC__OggEncoderAspectWriteCallbackProxy)encoder->private_->write_callback,
			encoder,
			encoder->private_->client_data
		);
	}
	else
#endif
	status = encoder->private_->write_callback(encoder, buffer, bytes, samples, encoder->private_->current_frame_number, encoder->private_->client_data);

	if(status == FLAC__STREAM_ENCODER_WRITE_STATUS_OK) {
		encoder->private_->bytes_written += bytes;
		encoder->private_->samples_written += samples;
		/* we keep a high watermark on the number of frames written because
		 * when the encoder goes back to write metadata, 'current_frame'
		 * will drop back to 0.
		 */
		encoder->private_->frames_written = flac_max(encoder->private_->frames_written, encoder->private_->current_frame_number+1);
	}
	else
		encoder->protected_->state = FLAC__STREAM_ENCODER_CLIENT_ERROR;

	return status;
}