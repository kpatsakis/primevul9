eb_get_vma(const struct i915_execbuffer *eb, unsigned long handle)
{
	if (eb->lut_size < 0) {
		if (handle >= -eb->lut_size)
			return NULL;
		return eb->vma[handle];
	} else {
		struct hlist_head *head;
		struct i915_vma *vma;

		head = &eb->buckets[hash_32(handle, eb->lut_size)];
		hlist_for_each_entry(vma, head, exec_node) {
			if (vma->exec_handle == handle)
				return vma;
		}
		return NULL;
	}
}