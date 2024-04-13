urandom_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	unsigned long flags;
	static int maxwarn = 10;

	if (!crng_ready() && maxwarn > 0) {
		maxwarn--;
		if (__ratelimit(&urandom_warning))
			pr_notice("%s: uninitialized urandom read (%zd bytes read)\n",
				  current->comm, nbytes);
		spin_lock_irqsave(&primary_crng.lock, flags);
		crng_init_cnt = 0;
		spin_unlock_irqrestore(&primary_crng.lock, flags);
	}

	return urandom_read_nowarn(file, buf, nbytes, ppos);
}