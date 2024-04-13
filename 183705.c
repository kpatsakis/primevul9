FLAC__bool process_frame_(FLAC__StreamEncoder *encoder, FLAC__bool is_fractional_block, FLAC__bool is_last_block)
{
	FLAC__uint16 crc;
	FLAC__ASSERT(encoder->protected_->state == FLAC__STREAM_ENCODER_OK);

	/*
	 * Accumulate raw signal to the MD5 signature
	 */
	if(encoder->protected_->do_md5 && !FLAC__MD5Accumulate(&encoder->private_->md5context, (const FLAC__int32 * const *)encoder->private_->integer_signal, encoder->protected_->channels, encoder->protected_->blocksize, (encoder->protected_->bits_per_sample+7) / 8)) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR;
		return false;
	}

	/*
	 * Process the frame header and subframes into the frame bitbuffer
	 */
	if(!process_subframes_(encoder, is_fractional_block)) {
		/* the above function sets the state for us in case of an error */
		return false;
	}

	/*
	 * Zero-pad the frame to a byte_boundary
	 */
	if(!FLAC__bitwriter_zero_pad_to_byte_boundary(encoder->private_->frame)) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR;
		return false;
	}

	/*
	 * CRC-16 the whole thing
	 */
	FLAC__ASSERT(FLAC__bitwriter_is_byte_aligned(encoder->private_->frame));
	if(
		!FLAC__bitwriter_get_write_crc16(encoder->private_->frame, &crc) ||
		!FLAC__bitwriter_write_raw_uint32(encoder->private_->frame, crc, FLAC__FRAME_FOOTER_CRC_LEN)
	) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR;
		return false;
	}

	/*
	 * Write it
	 */
	if(!write_bitbuffer_(encoder, encoder->protected_->blocksize, is_last_block)) {
		/* the above function sets the state for us in case of an error */
		return false;
	}

	/*
	 * Get ready for the next frame
	 */
	encoder->private_->current_sample_number = 0;
	encoder->private_->current_frame_number++;
	encoder->private_->streaminfo.data.stream_info.total_samples += (FLAC__uint64)encoder->protected_->blocksize;

	return true;
}