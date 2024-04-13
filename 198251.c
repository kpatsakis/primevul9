i915_gem_execbuffer_relocate_object_slow(struct drm_i915_gem_object *obj,
					 struct eb_objects *eb,
					 struct drm_i915_gem_relocation_entry *relocs)
{
	const struct drm_i915_gem_exec_object2 *entry = obj->exec_entry;
	int i, ret;

	for (i = 0; i < entry->relocation_count; i++) {
		ret = i915_gem_execbuffer_relocate_entry(obj, eb, &relocs[i]);
		if (ret)
			return ret;
	}

	return 0;
}