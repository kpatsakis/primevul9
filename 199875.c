int compat_core_sys_select(int n, compat_ulong_t __user *inp,
	compat_ulong_t __user *outp, compat_ulong_t __user *exp, s64 *timeout)
{
	fd_set_bits fds;
	char *bits;
	int size, max_fdset, ret = -EINVAL;
	struct fdtable *fdt;

	if (n < 0)
		goto out_nofds;

	/* max_fdset can increase, so grab it once to avoid race */
	rcu_read_lock();
	fdt = files_fdtable(current->files);
	max_fdset = fdt->max_fdset;
	rcu_read_unlock();
	if (n > max_fdset)
		n = max_fdset;

	/*
	 * We need 6 bitmaps (in/out/ex for both incoming and outgoing),
	 * since we used fdset we need to allocate memory in units of
	 * long-words.
	 */
	ret = -ENOMEM;
	size = FDS_BYTES(n);
	bits = kmalloc(6 * size, GFP_KERNEL);
	if (!bits)
		goto out_nofds;
	fds.in      = (unsigned long *)  bits;
	fds.out     = (unsigned long *) (bits +   size);
	fds.ex      = (unsigned long *) (bits + 2*size);
	fds.res_in  = (unsigned long *) (bits + 3*size);
	fds.res_out = (unsigned long *) (bits + 4*size);
	fds.res_ex  = (unsigned long *) (bits + 5*size);

	if ((ret = compat_get_fd_set(n, inp, fds.in)) ||
	    (ret = compat_get_fd_set(n, outp, fds.out)) ||
	    (ret = compat_get_fd_set(n, exp, fds.ex)))
		goto out;
	zero_fd_set(n, fds.res_in);
	zero_fd_set(n, fds.res_out);
	zero_fd_set(n, fds.res_ex);

	ret = do_select(n, &fds, timeout);

	if (ret < 0)
		goto out;
	if (!ret) {
		ret = -ERESTARTNOHAND;
		if (signal_pending(current))
			goto out;
		ret = 0;
	}

	compat_set_fd_set(n, inp, fds.res_in);
	compat_set_fd_set(n, outp, fds.res_out);
	compat_set_fd_set(n, exp, fds.res_ex);

out:
	kfree(bits);
out_nofds:
	return ret;
}