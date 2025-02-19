validate_exec_list(struct drm_i915_gem_exec_object2 *exec,
		   int count)
{
	int i;
	int relocs_total = 0;
	int relocs_max = INT_MAX / sizeof(struct drm_i915_gem_relocation_entry);

	for (i = 0; i < count; i++) {
		char __user *ptr = (char __user *)(uintptr_t)exec[i].relocs_ptr;
		int length; /* limited by fault_in_pages_readable() */

		if (exec[i].flags & __EXEC_OBJECT_UNKNOWN_FLAGS)
			return -EINVAL;

		/* First check for malicious input causing overflow in
		 * the worst case where we need to allocate the entire
		 * relocation tree as a single array.
		 */
		if (exec[i].relocation_count > relocs_max - relocs_total)
			return -EINVAL;
		relocs_total += exec[i].relocation_count;

		length = exec[i].relocation_count *
			sizeof(struct drm_i915_gem_relocation_entry);
		/* we may also need to update the presumed offsets */
		if (!access_ok(VERIFY_WRITE, ptr, length))
			return -EFAULT;

		if (fault_in_multipages_readable(ptr, length))
			return -EFAULT;
	}

	return 0;
}