static void tty_ldisc_put(struct tty_ldisc *ld)
{
	if (WARN_ON_ONCE(!ld))
		return;

	put_ldops(ld->ops);
	kfree(ld);
}