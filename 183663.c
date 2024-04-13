FLAC__bool add_subframe_(
	FLAC__StreamEncoder *encoder,
	uint32_t blocksize,
	uint32_t subframe_bps,
	const FLAC__Subframe *subframe,
	FLAC__BitWriter *frame
)
{
	switch(subframe->type) {
		case FLAC__SUBFRAME_TYPE_CONSTANT:
			if(!FLAC__subframe_add_constant(&(subframe->data.constant), subframe_bps, subframe->wasted_bits, frame)) {
				encoder->protected_->state = FLAC__STREAM_ENCODER_FRAMING_ERROR;
				return false;
			}
			break;
		case FLAC__SUBFRAME_TYPE_FIXED:
			if(!FLAC__subframe_add_fixed(&(subframe->data.fixed), blocksize - subframe->data.fixed.order, subframe_bps, subframe->wasted_bits, frame)) {
				encoder->protected_->state = FLAC__STREAM_ENCODER_FRAMING_ERROR;
				return false;
			}
			break;
		case FLAC__SUBFRAME_TYPE_LPC:
			if(!FLAC__subframe_add_lpc(&(subframe->data.lpc), blocksize - subframe->data.lpc.order, subframe_bps, subframe->wasted_bits, frame)) {
				encoder->protected_->state = FLAC__STREAM_ENCODER_FRAMING_ERROR;
				return false;
			}
			break;
		case FLAC__SUBFRAME_TYPE_VERBATIM:
			if(!FLAC__subframe_add_verbatim(&(subframe->data.verbatim), blocksize, subframe_bps, subframe->wasted_bits, frame)) {
				encoder->protected_->state = FLAC__STREAM_ENCODER_FRAMING_ERROR;
				return false;
			}
			break;
		default:
			FLAC__ASSERT(0);
	}

	return true;
}