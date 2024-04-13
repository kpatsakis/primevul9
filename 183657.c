FLAC__bool resize_buffers_(FLAC__StreamEncoder *encoder, uint32_t new_blocksize)
{
	FLAC__bool ok;
	uint32_t i, channel;

	FLAC__ASSERT(new_blocksize > 0);
	FLAC__ASSERT(encoder->protected_->state == FLAC__STREAM_ENCODER_OK);
	FLAC__ASSERT(encoder->private_->current_sample_number == 0);

	/* To avoid excessive malloc'ing, we only grow the buffer; no shrinking. */
	if(new_blocksize <= encoder->private_->input_capacity)
		return true;

	ok = true;

	/* WATCHOUT: FLAC__lpc_compute_residual_from_qlp_coefficients_asm_ia32_mmx() and ..._intrin_sse2()
	 * require that the input arrays (in our case the integer signals)
	 * have a buffer of up to 3 zeroes in front (at negative indices) for
	 * alignment purposes; we use 4 in front to keep the data well-aligned.
	 */

	for(i = 0; ok && i < encoder->protected_->channels; i++) {
		ok = ok && FLAC__memory_alloc_aligned_int32_array(new_blocksize+4+OVERREAD_, &encoder->private_->integer_signal_unaligned[i], &encoder->private_->integer_signal[i]);
		memset(encoder->private_->integer_signal[i], 0, sizeof(FLAC__int32)*4);
		encoder->private_->integer_signal[i] += 4;
#ifndef FLAC__INTEGER_ONLY_LIBRARY
#if 0 /* @@@ currently unused */
		if(encoder->protected_->max_lpc_order > 0)
			ok = ok && FLAC__memory_alloc_aligned_real_array(new_blocksize+OVERREAD_, &encoder->private_->real_signal_unaligned[i], &encoder->private_->real_signal[i]);
#endif
#endif
	}
	for(i = 0; ok && i < 2; i++) {
		ok = ok && FLAC__memory_alloc_aligned_int32_array(new_blocksize+4+OVERREAD_, &encoder->private_->integer_signal_mid_side_unaligned[i], &encoder->private_->integer_signal_mid_side[i]);
		memset(encoder->private_->integer_signal_mid_side[i], 0, sizeof(FLAC__int32)*4);
		encoder->private_->integer_signal_mid_side[i] += 4;
#ifndef FLAC__INTEGER_ONLY_LIBRARY
#if 0 /* @@@ currently unused */
		if(encoder->protected_->max_lpc_order > 0)
			ok = ok && FLAC__memory_alloc_aligned_real_array(new_blocksize+OVERREAD_, &encoder->private_->real_signal_mid_side_unaligned[i], &encoder->private_->real_signal_mid_side[i]);
#endif
#endif
	}
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	if(ok && encoder->protected_->max_lpc_order > 0) {
		for(i = 0; ok && i < encoder->protected_->num_apodizations; i++)
			ok = ok && FLAC__memory_alloc_aligned_real_array(new_blocksize, &encoder->private_->window_unaligned[i], &encoder->private_->window[i]);
		ok = ok && FLAC__memory_alloc_aligned_real_array(new_blocksize, &encoder->private_->windowed_signal_unaligned, &encoder->private_->windowed_signal);
	}
#endif
	for(channel = 0; ok && channel < encoder->protected_->channels; channel++) {
		for(i = 0; ok && i < 2; i++) {
			ok = ok && FLAC__memory_alloc_aligned_int32_array(new_blocksize, &encoder->private_->residual_workspace_unaligned[channel][i], &encoder->private_->residual_workspace[channel][i]);
		}
	}
	for(channel = 0; ok && channel < 2; channel++) {
		for(i = 0; ok && i < 2; i++) {
			ok = ok && FLAC__memory_alloc_aligned_int32_array(new_blocksize, &encoder->private_->residual_workspace_mid_side_unaligned[channel][i], &encoder->private_->residual_workspace_mid_side[channel][i]);
		}
	}
	/* the *2 is an approximation to the series 1 + 1/2 + 1/4 + ... that sums tree occupies in a flat array */
	/*@@@ new_blocksize*2 is too pessimistic, but to fix, we need smarter logic because a smaller new_blocksize can actually increase the # of partitions; would require moving this out into a separate function, then checking its capacity against the need of the current blocksize&min/max_partition_order (and maybe predictor order) */
	ok = ok && FLAC__memory_alloc_aligned_uint64_array(new_blocksize * 2, &encoder->private_->abs_residual_partition_sums_unaligned, &encoder->private_->abs_residual_partition_sums);
	if(encoder->protected_->do_escape_coding)
		ok = ok && FLAC__memory_alloc_aligned_unsigned_array(new_blocksize * 2, &encoder->private_->raw_bits_per_partition_unaligned, &encoder->private_->raw_bits_per_partition);

	/* now adjust the windows if the blocksize has changed */
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	if(ok && new_blocksize != encoder->private_->input_capacity && encoder->protected_->max_lpc_order > 0) {
		for(i = 0; ok && i < encoder->protected_->num_apodizations; i++) {
			switch(encoder->protected_->apodizations[i].type) {
				case FLAC__APODIZATION_BARTLETT:
					FLAC__window_bartlett(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_BARTLETT_HANN:
					FLAC__window_bartlett_hann(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_BLACKMAN:
					FLAC__window_blackman(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_BLACKMAN_HARRIS_4TERM_92DB_SIDELOBE:
					FLAC__window_blackman_harris_4term_92db_sidelobe(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_CONNES:
					FLAC__window_connes(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_FLATTOP:
					FLAC__window_flattop(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_GAUSS:
					FLAC__window_gauss(encoder->private_->window[i], new_blocksize, encoder->protected_->apodizations[i].parameters.gauss.stddev);
					break;
				case FLAC__APODIZATION_HAMMING:
					FLAC__window_hamming(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_HANN:
					FLAC__window_hann(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_KAISER_BESSEL:
					FLAC__window_kaiser_bessel(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_NUTTALL:
					FLAC__window_nuttall(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_RECTANGLE:
					FLAC__window_rectangle(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_TRIANGLE:
					FLAC__window_triangle(encoder->private_->window[i], new_blocksize);
					break;
				case FLAC__APODIZATION_TUKEY:
					FLAC__window_tukey(encoder->private_->window[i], new_blocksize, encoder->protected_->apodizations[i].parameters.tukey.p);
					break;
				case FLAC__APODIZATION_PARTIAL_TUKEY:
					FLAC__window_partial_tukey(encoder->private_->window[i], new_blocksize, encoder->protected_->apodizations[i].parameters.multiple_tukey.p, encoder->protected_->apodizations[i].parameters.multiple_tukey.start, encoder->protected_->apodizations[i].parameters.multiple_tukey.end);
					break;
				case FLAC__APODIZATION_PUNCHOUT_TUKEY:
					FLAC__window_punchout_tukey(encoder->private_->window[i], new_blocksize, encoder->protected_->apodizations[i].parameters.multiple_tukey.p, encoder->protected_->apodizations[i].parameters.multiple_tukey.start, encoder->protected_->apodizations[i].parameters.multiple_tukey.end);
					break;
				case FLAC__APODIZATION_WELCH:
					FLAC__window_welch(encoder->private_->window[i], new_blocksize);
					break;
				default:
					FLAC__ASSERT(0);
					/* double protection */
					FLAC__window_hann(encoder->private_->window[i], new_blocksize);
					break;
			}
		}
	}
#endif

	if(ok)
		encoder->private_->input_capacity = new_blocksize;
	else
		encoder->protected_->state = FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR;

	return ok;
}