FLAC__bool process_subframe_(
	FLAC__StreamEncoder *encoder,
	uint32_t min_partition_order,
	uint32_t max_partition_order,
	const FLAC__FrameHeader *frame_header,
	uint32_t subframe_bps,
	const FLAC__int32 integer_signal[],
	FLAC__Subframe *subframe[2],
	FLAC__EntropyCodingMethod_PartitionedRiceContents *partitioned_rice_contents[2],
	FLAC__int32 *residual[2],
	uint32_t *best_subframe,
	uint32_t *best_bits
)
{
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	float fixed_residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1];
#else
	FLAC__fixedpoint fixed_residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1];
#endif
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	double lpc_residual_bits_per_sample;
	FLAC__real autoc[FLAC__MAX_LPC_ORDER+1]; /* WATCHOUT: the size is important even though encoder->protected_->max_lpc_order might be less; some asm and x86 intrinsic routines need all the space */
	double lpc_error[FLAC__MAX_LPC_ORDER];
	uint32_t min_lpc_order, max_lpc_order, lpc_order;
	uint32_t min_qlp_coeff_precision, max_qlp_coeff_precision, qlp_coeff_precision;
#endif
	uint32_t min_fixed_order, max_fixed_order, guess_fixed_order, fixed_order;
	uint32_t rice_parameter;
	uint32_t _candidate_bits, _best_bits;
	uint32_t _best_subframe;
	/* only use RICE2 partitions if stream bps > 16 */
	const uint32_t rice_parameter_limit = FLAC__stream_encoder_get_bits_per_sample(encoder) > 16? FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_ESCAPE_PARAMETER : FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ESCAPE_PARAMETER;

	FLAC__ASSERT(frame_header->blocksize > 0);

	/* verbatim subframe is the baseline against which we measure other compressed subframes */
	_best_subframe = 0;
	if(encoder->private_->disable_verbatim_subframes && frame_header->blocksize >= FLAC__MAX_FIXED_ORDER)
		_best_bits = UINT_MAX;
	else
		_best_bits = evaluate_verbatim_subframe_(encoder, integer_signal, frame_header->blocksize, subframe_bps, subframe[_best_subframe]);

	if(frame_header->blocksize >= FLAC__MAX_FIXED_ORDER) {
		uint32_t signal_is_constant = false;
		if(subframe_bps + 4 + FLAC__bitmath_ilog2((frame_header->blocksize-FLAC__MAX_FIXED_ORDER)|1) <= 32)
			guess_fixed_order = encoder->private_->local_fixed_compute_best_predictor(integer_signal+FLAC__MAX_FIXED_ORDER, frame_header->blocksize-FLAC__MAX_FIXED_ORDER, fixed_residual_bits_per_sample);
		else
			guess_fixed_order = encoder->private_->local_fixed_compute_best_predictor_wide(integer_signal+FLAC__MAX_FIXED_ORDER, frame_header->blocksize-FLAC__MAX_FIXED_ORDER, fixed_residual_bits_per_sample);
		/* check for constant subframe */
		if(
			!encoder->private_->disable_constant_subframes &&
#ifndef FLAC__INTEGER_ONLY_LIBRARY
			fixed_residual_bits_per_sample[1] == 0.0
#else
			fixed_residual_bits_per_sample[1] == FLAC__FP_ZERO
#endif
		) {
			/* the above means it's possible all samples are the same value; now double-check it: */
			uint32_t i;
			signal_is_constant = true;
			for(i = 1; i < frame_header->blocksize; i++) {
				if(integer_signal[0] != integer_signal[i]) {
					signal_is_constant = false;
					break;
				}
			}
		}
		if(signal_is_constant) {
			_candidate_bits = evaluate_constant_subframe_(encoder, integer_signal[0], frame_header->blocksize, subframe_bps, subframe[!_best_subframe]);
			if(_candidate_bits < _best_bits) {
				_best_subframe = !_best_subframe;
				_best_bits = _candidate_bits;
			}
		}
		else {
			if(!encoder->private_->disable_fixed_subframes || (encoder->protected_->max_lpc_order == 0 && _best_bits == UINT_MAX)) {
				/* encode fixed */
				if(encoder->protected_->do_exhaustive_model_search) {
					min_fixed_order = 0;
					max_fixed_order = FLAC__MAX_FIXED_ORDER;
				}
				else {
					min_fixed_order = max_fixed_order = guess_fixed_order;
				}
				if(max_fixed_order >= frame_header->blocksize)
					max_fixed_order = frame_header->blocksize - 1;
				for(fixed_order = min_fixed_order; fixed_order <= max_fixed_order; fixed_order++) {
#ifndef FLAC__INTEGER_ONLY_LIBRARY
					if(fixed_residual_bits_per_sample[fixed_order] >= (float)subframe_bps)
						continue; /* don't even try */
					rice_parameter = (fixed_residual_bits_per_sample[fixed_order] > 0.0)? (uint32_t)(fixed_residual_bits_per_sample[fixed_order]+0.5) : 0; /* 0.5 is for rounding */
#else
					if(FLAC__fixedpoint_trunc(fixed_residual_bits_per_sample[fixed_order]) >= (int)subframe_bps)
						continue; /* don't even try */
					rice_parameter = (fixed_residual_bits_per_sample[fixed_order] > FLAC__FP_ZERO)? (uint32_t)FLAC__fixedpoint_trunc(fixed_residual_bits_per_sample[fixed_order]+FLAC__FP_ONE_HALF) : 0; /* 0.5 is for rounding */
#endif
					rice_parameter++; /* to account for the signed->uint32_t conversion during rice coding */
					if(rice_parameter >= rice_parameter_limit) {
#ifndef NDEBUG
						fprintf(stderr, "clipping rice_parameter (%u -> %u) @0\n", rice_parameter, rice_parameter_limit - 1);
#endif
						rice_parameter = rice_parameter_limit - 1;
					}
					_candidate_bits =
						evaluate_fixed_subframe_(
							encoder,
							integer_signal,
							residual[!_best_subframe],
							encoder->private_->abs_residual_partition_sums,
							encoder->private_->raw_bits_per_partition,
							frame_header->blocksize,
							subframe_bps,
							fixed_order,
							rice_parameter,
							rice_parameter_limit,
							min_partition_order,
							max_partition_order,
							encoder->protected_->do_escape_coding,
							encoder->protected_->rice_parameter_search_dist,
							subframe[!_best_subframe],
							partitioned_rice_contents[!_best_subframe]
						);
					if(_candidate_bits < _best_bits) {
						_best_subframe = !_best_subframe;
						_best_bits = _candidate_bits;
					}
				}
			}

#ifndef FLAC__INTEGER_ONLY_LIBRARY
			/* encode lpc */
			if(encoder->protected_->max_lpc_order > 0) {
				if(encoder->protected_->max_lpc_order >= frame_header->blocksize)
					max_lpc_order = frame_header->blocksize-1;
				else
					max_lpc_order = encoder->protected_->max_lpc_order;
				if(max_lpc_order > 0) {
					uint32_t a;
					for (a = 0; a < encoder->protected_->num_apodizations; a++) {
						FLAC__lpc_window_data(integer_signal, encoder->private_->window[a], encoder->private_->windowed_signal, frame_header->blocksize);
						encoder->private_->local_lpc_compute_autocorrelation(encoder->private_->windowed_signal, frame_header->blocksize, max_lpc_order+1, autoc);
						/* if autoc[0] == 0.0, the signal is constant and we usually won't get here, but it can happen */
						if(autoc[0] != 0.0) {
							FLAC__lpc_compute_lp_coefficients(autoc, &max_lpc_order, encoder->private_->lp_coeff, lpc_error);
							if(encoder->protected_->do_exhaustive_model_search) {
								min_lpc_order = 1;
							}
							else {
								const uint32_t guess_lpc_order =
									FLAC__lpc_compute_best_order(
										lpc_error,
										max_lpc_order,
										frame_header->blocksize,
										subframe_bps + (
											encoder->protected_->do_qlp_coeff_prec_search?
												FLAC__MIN_QLP_COEFF_PRECISION : /* have to guess; use the min possible size to avoid accidentally favoring lower orders */
												encoder->protected_->qlp_coeff_precision
										)
									);
								min_lpc_order = max_lpc_order = guess_lpc_order;
							}
							if(max_lpc_order >= frame_header->blocksize)
								max_lpc_order = frame_header->blocksize - 1;
							for(lpc_order = min_lpc_order; lpc_order <= max_lpc_order; lpc_order++) {
								lpc_residual_bits_per_sample = FLAC__lpc_compute_expected_bits_per_residual_sample(lpc_error[lpc_order-1], frame_header->blocksize-lpc_order);
								if(lpc_residual_bits_per_sample >= (double)subframe_bps)
									continue; /* don't even try */
								rice_parameter = (lpc_residual_bits_per_sample > 0.0)? (uint32_t)(lpc_residual_bits_per_sample+0.5) : 0; /* 0.5 is for rounding */
								rice_parameter++; /* to account for the signed->uint32_t conversion during rice coding */
								if(rice_parameter >= rice_parameter_limit) {
#ifndef NDEBUG
									fprintf(stderr, "clipping rice_parameter (%u -> %u) @1\n", rice_parameter, rice_parameter_limit - 1);
#endif
									rice_parameter = rice_parameter_limit - 1;
								}
								if(encoder->protected_->do_qlp_coeff_prec_search) {
									min_qlp_coeff_precision = FLAC__MIN_QLP_COEFF_PRECISION;
									/* try to keep qlp coeff precision such that only 32-bit math is required for decode of <=16bps(+1bps for side channel) streams */
									if(subframe_bps <= 17) {
										max_qlp_coeff_precision = flac_min(32 - subframe_bps - FLAC__bitmath_ilog2(lpc_order), FLAC__MAX_QLP_COEFF_PRECISION);
										max_qlp_coeff_precision = flac_max(max_qlp_coeff_precision, min_qlp_coeff_precision);
									}
									else
										max_qlp_coeff_precision = FLAC__MAX_QLP_COEFF_PRECISION;
								}
								else {
									min_qlp_coeff_precision = max_qlp_coeff_precision = encoder->protected_->qlp_coeff_precision;
								}
								for(qlp_coeff_precision = min_qlp_coeff_precision; qlp_coeff_precision <= max_qlp_coeff_precision; qlp_coeff_precision++) {
									_candidate_bits =
										evaluate_lpc_subframe_(
											encoder,
											integer_signal,
											residual[!_best_subframe],
											encoder->private_->abs_residual_partition_sums,
											encoder->private_->raw_bits_per_partition,
											encoder->private_->lp_coeff[lpc_order-1],
											frame_header->blocksize,
											subframe_bps,
											lpc_order,
											qlp_coeff_precision,
											rice_parameter,
											rice_parameter_limit,
											min_partition_order,
											max_partition_order,
											encoder->protected_->do_escape_coding,
											encoder->protected_->rice_parameter_search_dist,
											subframe[!_best_subframe],
											partitioned_rice_contents[!_best_subframe]
										);
									if(_candidate_bits > 0) { /* if == 0, there was a problem quantizing the lpcoeffs */
										if(_candidate_bits < _best_bits) {
											_best_subframe = !_best_subframe;
											_best_bits = _candidate_bits;
										}
									}
								}
							}
						}
					}
				}
			}
#endif /* !defined FLAC__INTEGER_ONLY_LIBRARY */
		}
	}

	/* under rare circumstances this can happen when all but lpc subframe types are disabled: */
	if(_best_bits == UINT_MAX) {
		FLAC__ASSERT(_best_subframe == 0);
		_best_bits = evaluate_verbatim_subframe_(encoder, integer_signal, frame_header->blocksize, subframe_bps, subframe[_best_subframe]);
	}

	*best_subframe = _best_subframe;
	*best_bits = _best_bits;

	return true;
}