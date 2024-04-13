long compat_get_bitmap(unsigned long *mask, const compat_ulong_t __user *umask,
		       unsigned long bitmap_size)
{
	int i, j;
	unsigned long m;
	compat_ulong_t um;
	unsigned long nr_compat_longs;

	/* align bitmap up to nearest compat_long_t boundary */
	bitmap_size = ALIGN(bitmap_size, BITS_PER_COMPAT_LONG);

	if (!access_ok(VERIFY_READ, umask, bitmap_size / 8))
		return -EFAULT;

	nr_compat_longs = BITS_TO_COMPAT_LONGS(bitmap_size);

	for (i = 0; i < BITS_TO_LONGS(bitmap_size); i++) {
		m = 0;

		for (j = 0; j < sizeof(m)/sizeof(um); j++) {
			/*
			 * We dont want to read past the end of the userspace
			 * bitmap. We must however ensure the end of the
			 * kernel bitmap is zeroed.
			 */
			if (nr_compat_longs) {
				nr_compat_longs--;
				if (__get_user(um, umask))
					return -EFAULT;
			} else {
				um = 0;
			}

			umask++;
			m |= (long)um << (j * BITS_PER_COMPAT_LONG);
		}
		*mask++ = m;
	}

	return 0;
}