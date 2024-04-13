static long random_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	int size, ent_count;
	int __user *p = (int __user *)arg;
	int retval;

	switch (cmd) {
	case RNDGETENTCNT:
		/* inherently racy, no point locking */
		ent_count = ENTROPY_BITS(&input_pool);
		if (put_user(ent_count, p))
			return -EFAULT;
		return 0;
	case RNDADDTOENTCNT:
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
		if (get_user(ent_count, p))
			return -EFAULT;
		return credit_entropy_bits_safe(&input_pool, ent_count);
	case RNDADDENTROPY:
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
		if (get_user(ent_count, p++))
			return -EFAULT;
		if (ent_count < 0)
			return -EINVAL;
		if (get_user(size, p++))
			return -EFAULT;
		retval = write_pool(&input_pool, (const char __user *)p,
				    size);
		if (retval < 0)
			return retval;
		return credit_entropy_bits_safe(&input_pool, ent_count);
	case RNDZAPENTCNT:
	case RNDCLEARPOOL:
		/*
		 * Clear the entropy pool counters. We no longer clear
		 * the entropy pool, as that's silly.
		 */
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
		input_pool.entropy_count = 0;
		return 0;
	case RNDRESEEDCRNG:
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
		if (crng_init < 2)
			return -ENODATA;
		crng_reseed(&primary_crng, NULL);
		crng_global_init_time = jiffies - 1;
		return 0;
	default:
		return -EINVAL;
	}
}