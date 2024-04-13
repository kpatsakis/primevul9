static bool i915_gem_check_execbuffer(struct drm_i915_gem_execbuffer2 *exec)
{
	if (exec->flags & __I915_EXEC_ILLEGAL_FLAGS)
		return false;

	/* Kernel clipping was a DRI1 misfeature */
	if (!(exec->flags & I915_EXEC_FENCE_ARRAY)) {
		if (exec->num_cliprects || exec->cliprects_ptr)
			return false;
	}

	if (exec->DR4 == 0xffffffff) {
		DRM_DEBUG("UXA submitting garbage DR4, fixing up\n");
		exec->DR4 = 0;
	}
	if (exec->DR1 || exec->DR4)
		return false;

	if ((exec->batch_start_offset | exec->batch_len) & 0x7)
		return false;

	return true;
}