static struct tty_ldisc_ops *get_ldops(int disc)
{
	unsigned long flags;
	struct tty_ldisc_ops *ldops, *ret;

	raw_spin_lock_irqsave(&tty_ldiscs_lock, flags);
	ret = ERR_PTR(-EINVAL);
	ldops = tty_ldiscs[disc];
	if (ldops) {
		ret = ERR_PTR(-EAGAIN);
		if (try_module_get(ldops->owner)) {
			ldops->refcount++;
			ret = ldops;
		}
	}
	raw_spin_unlock_irqrestore(&tty_ldiscs_lock, flags);
	return ret;
}