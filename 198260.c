i915_gem_execbuffer_unreserve_object(struct drm_i915_gem_object *obj)
{
	struct drm_i915_gem_exec_object2 *entry;

	if (!obj->gtt_space)
		return;

	entry = obj->exec_entry;

	if (entry->flags & __EXEC_OBJECT_HAS_FENCE)
		i915_gem_object_unpin_fence(obj);

	if (entry->flags & __EXEC_OBJECT_HAS_PIN)
		i915_gem_object_unpin(obj);

	entry->flags &= ~(__EXEC_OBJECT_HAS_FENCE | __EXEC_OBJECT_HAS_PIN);
}