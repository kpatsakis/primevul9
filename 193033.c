long compat_put_bitmap(compat_ulong_t __user *umask, unsigned long *mask,
		       unsigned long bitmap_size)
{
	int i, j;
	unsigned long m;
	compat_ulong_t um;
	unsigned long nr_compat_longs;

	/* align bitmap up to nearest compat_long_t boundary */
	bitmap_size = ALIGN(bitmap_size, BITS_PER_COMPAT_LONG);

	if (!access_ok(VERIFY_WRITE, umask, bitmap_size / 8))
		return -EFAULT;

	nr_compat_longs = BITS_TO_COMPAT_LONGS(bitmap_size);

	for (i = 0; i < BITS_TO_LONGS(bitmap_size); i++) {
		m = *mask++;

		for (j = 0; j < sizeof(m)/sizeof(um); j++) {
			um = m;

			/*
			 * We dont want to write past the end of the userspace
			 * bitmap.
			 */
			if (nr_compat_longs) {
				nr_compat_longs--;
				if (__put_user(um, umask))
					return -EFAULT;
			}

			umask++;
			m >>= 4*sizeof(um);
			m >>= 4*sizeof(um);
		}
	}

	return 0;
}