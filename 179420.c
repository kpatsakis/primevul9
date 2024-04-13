void __user *compat_alloc_user_space(unsigned long len)
{
	void __user *ptr;

	/* If len would occupy more than half of the entire compat space... */
	if (unlikely(len > (((compat_uptr_t)~0) >> 1)))
		return NULL;

	ptr = arch_compat_alloc_user_space(len);

	if (unlikely(!access_ok(ptr, len)))
		return NULL;

	return ptr;
}