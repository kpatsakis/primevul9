static void spotcheck_subframe_estimate_(
	FLAC__StreamEncoder *encoder,
	uint32_t blocksize,
	uint32_t subframe_bps,
	const FLAC__Subframe *subframe,
	uint32_t estimate
)
{
	FLAC__bool ret;
	FLAC__BitWriter *frame = FLAC__bitwriter_new();
	if(frame == 0) {
		fprintf(stderr, "EST: can't allocate frame\n");
		return;
	}
	if(!FLAC__bitwriter_init(frame)) {
		fprintf(stderr, "EST: can't init frame\n");
		return;
	}
	ret = add_subframe_(encoder, blocksize, subframe_bps, subframe, frame);
	FLAC__ASSERT(ret);
	{
		const uint32_t actual = FLAC__bitwriter_get_input_bits_unconsumed(frame);
		if(estimate != actual)
			fprintf(stderr, "EST: bad, frame#%u sub#%%d type=%8s est=%u, actual=%u, delta=%d\n", encoder->private_->current_frame_number, FLAC__SubframeTypeString[subframe->type], estimate, actual, (int)actual-(int)estimate);
	}
	FLAC__bitwriter_delete(frame);
}