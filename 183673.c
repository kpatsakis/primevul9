FLAC_API FLAC__bool FLAC__stream_encoder_finish(FLAC__StreamEncoder *encoder)
{
	FLAC__bool error = false;

	if (encoder == NULL)
		return false;

	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);

	if(encoder->protected_->state == FLAC__STREAM_ENCODER_UNINITIALIZED)
		return true;

	if(encoder->protected_->state == FLAC__STREAM_ENCODER_OK && !encoder->private_->is_being_deleted) {
		if(encoder->private_->current_sample_number != 0) {
			const FLAC__bool is_fractional_block = encoder->protected_->blocksize != encoder->private_->current_sample_number;
			encoder->protected_->blocksize = encoder->private_->current_sample_number;
			if(!process_frame_(encoder, is_fractional_block, /*is_last_block=*/true))
				error = true;
		}
	}

	if(encoder->protected_->do_md5)
		FLAC__MD5Final(encoder->private_->streaminfo.data.stream_info.md5sum, &encoder->private_->md5context);

	if(!encoder->private_->is_being_deleted) {
		if(encoder->protected_->state == FLAC__STREAM_ENCODER_OK) {
			if(encoder->private_->seek_callback) {
#if FLAC__HAS_OGG
				if(encoder->private_->is_ogg)
					update_ogg_metadata_(encoder);
				else
#endif
				update_metadata_(encoder);

				/* check if an error occurred while updating metadata */
				if(encoder->protected_->state != FLAC__STREAM_ENCODER_OK)
					error = true;
			}
			if(encoder->private_->metadata_callback)
				encoder->private_->metadata_callback(encoder, &encoder->private_->streaminfo, encoder->private_->client_data);
		}

		if(encoder->protected_->verify && 0 != encoder->private_->verify.decoder && !FLAC__stream_decoder_finish(encoder->private_->verify.decoder)) {
			if(!error)
				encoder->protected_->state = FLAC__STREAM_ENCODER_VERIFY_MISMATCH_IN_AUDIO_DATA;
			error = true;
		}
	}

	if(0 != encoder->private_->file) {
		if(encoder->private_->file != stdout)
			fclose(encoder->private_->file);
		encoder->private_->file = 0;
	}

#if FLAC__HAS_OGG
	if(encoder->private_->is_ogg)
		FLAC__ogg_encoder_aspect_finish(&encoder->protected_->ogg_encoder_aspect);
#endif

	free_(encoder);
	set_defaults_(encoder);

	if(!error)
		encoder->protected_->state = FLAC__STREAM_ENCODER_UNINITIALIZED;

	return !error;
}