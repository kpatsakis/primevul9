static int eb_submit(struct i915_execbuffer *eb)
{
	int err;

	err = eb_move_to_gpu(eb);
	if (err)
		return err;

	if (eb->args->flags & I915_EXEC_GEN7_SOL_RESET) {
		err = i915_reset_gen7_sol_offsets(eb->request);
		if (err)
			return err;
	}

	err = eb->engine->emit_bb_start(eb->request,
					eb->batch->node.start +
					eb->batch_start_offset,
					eb->batch_len,
					eb->batch_flags);
	if (err)
		return err;

	return 0;
}