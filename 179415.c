static int check_relocations(const struct drm_i915_gem_exec_object2 *entry)
{
	const char __user *addr, *end;
	unsigned long size;
	char __maybe_unused c;

	size = entry->relocation_count;
	if (size == 0)
		return 0;

	if (size > N_RELOC(ULONG_MAX))
		return -EINVAL;

	addr = u64_to_user_ptr(entry->relocs_ptr);
	size *= sizeof(struct drm_i915_gem_relocation_entry);
	if (!access_ok(addr, size))
		return -EFAULT;

	end = addr + size;
	for (; addr < end; addr += PAGE_SIZE) {
		int err = __get_user(c, addr);
		if (err)
			return err;
	}
	return __get_user(c, end - 1);
}