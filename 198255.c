i915_gem_check_execbuffer(struct drm_i915_gem_execbuffer2 *exec)
{
	if (exec->flags & __I915_EXEC_UNKNOWN_FLAGS)
		return false;

	return ((exec->batch_start_offset | exec->batch_len) & 0x7) == 0;
}