static ssize_t extract_crng_user(void __user *buf, size_t nbytes)
{
	ssize_t ret = 0, i = CHACHA_BLOCK_SIZE;
	__u8 tmp[CHACHA_BLOCK_SIZE] __aligned(4);
	int large_request = (nbytes > 256);

	while (nbytes) {
		if (large_request && need_resched()) {
			if (signal_pending(current)) {
				if (ret == 0)
					ret = -ERESTARTSYS;
				break;
			}
			schedule();
		}

		extract_crng(tmp);
		i = min_t(int, nbytes, CHACHA_BLOCK_SIZE);
		if (copy_to_user(buf, tmp, i)) {
			ret = -EFAULT;
			break;
		}

		nbytes -= i;
		buf += i;
		ret += i;
	}
	crng_backtrack_protect(tmp, i);

	/* Wipe data just written to memory */
	memzero_explicit(tmp, sizeof(tmp));

	return ret;
}