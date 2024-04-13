static void put_ldops(struct tty_ldisc_ops *ldops)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&tty_ldiscs_lock, flags);
	ldops->refcount--;
	module_put(ldops->owner);
	raw_spin_unlock_irqrestore(&tty_ldiscs_lock, flags);
}