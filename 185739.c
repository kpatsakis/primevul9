static int gen8_emit_rpcs_config(struct i915_request *rq,
				 struct intel_context *ce,
				 struct intel_sseu sseu)
{
	u64 offset;
	u32 *cs;

	cs = intel_ring_begin(rq, 4);
	if (IS_ERR(cs))
		return PTR_ERR(cs);

	offset = i915_ggtt_offset(ce->state) +
		 LRC_STATE_PN * PAGE_SIZE +
		 (CTX_R_PWR_CLK_STATE + 1) * 4;

	*cs++ = MI_STORE_DWORD_IMM_GEN4 | MI_USE_GGTT;
	*cs++ = lower_32_bits(offset);
	*cs++ = upper_32_bits(offset);
	*cs++ = gen8_make_rpcs(rq->i915, &sseu);

	intel_ring_advance(rq, cs);

	return 0;
}