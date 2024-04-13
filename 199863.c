int compat_get_fd_set(unsigned long nr, compat_ulong_t __user *ufdset,
			unsigned long *fdset)
{
	nr = ROUND_UP(nr, __COMPAT_NFDBITS);
	if (ufdset) {
		unsigned long odd;

		if (!access_ok(VERIFY_WRITE, ufdset, nr*sizeof(compat_ulong_t)))
			return -EFAULT;

		odd = nr & 1UL;
		nr &= ~1UL;
		while (nr) {
			unsigned long h, l;
			__get_user(l, ufdset);
			__get_user(h, ufdset+1);
			ufdset += 2;
			*fdset++ = h << 32 | l;
			nr -= 2;
		}
		if (odd)
			__get_user(*fdset, ufdset);
	} else {
		/* Tricky, must clear full unsigned long in the
		 * kernel fdset at the end, this makes sure that
		 * actually happens.
		 */
		memset(fdset, 0, ((nr + 1) & ~1)*sizeof(compat_ulong_t));
	}
	return 0;
}