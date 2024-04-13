relocation_target(const struct drm_i915_gem_relocation_entry *reloc,
		  const struct i915_vma *target)
{
	return gen8_canonical_addr((int)reloc->delta + target->node.start);
}