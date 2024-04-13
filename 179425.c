static int i915_reset_gen7_sol_offsets(struct i915_request *rq)
{
	u32 *cs;
	int i;

	if (!IS_GEN7(rq->i915) || rq->engine->id != RCS) {
		DRM_DEBUG("sol reset is gen7/rcs only\n");
		return -EINVAL;
	}

	cs = intel_ring_begin(rq, 4 * 2 + 2);
	if (IS_ERR(cs))
		return PTR_ERR(cs);

	*cs++ = MI_LOAD_REGISTER_IMM(4);
	for (i = 0; i < 4; i++) {
		*cs++ = i915_mmio_reg_offset(GEN7_SO_WRITE_OFFSET(i));
		*cs++ = 0;
	}
	*cs++ = MI_NOOP;
	intel_ring_advance(rq, cs);

	return 0;
}