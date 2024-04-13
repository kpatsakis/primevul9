i915_gem_execbuffer_move_to_active(struct list_head *objects,
				   struct intel_ring_buffer *ring)
{
	struct drm_i915_gem_object *obj;

	list_for_each_entry(obj, objects, exec_list) {
		u32 old_read = obj->base.read_domains;
		u32 old_write = obj->base.write_domain;

		obj->base.write_domain = obj->base.pending_write_domain;
		if (obj->base.write_domain == 0)
			obj->base.pending_read_domains |= obj->base.read_domains;
		obj->base.read_domains = obj->base.pending_read_domains;
		obj->fenced_gpu_access = obj->pending_fenced_gpu_access;

		i915_gem_object_move_to_active(obj, ring);
		if (obj->base.write_domain) {
			obj->dirty = 1;
			obj->last_write_seqno = intel_ring_get_seqno(ring);
			if (obj->pin_count) /* check for potential scanout */
				intel_mark_fb_busy(obj);
		}

		trace_i915_gem_object_change_domain(obj, old_read, old_write);
	}
}