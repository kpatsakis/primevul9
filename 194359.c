static void __lockfunc tty_ldisc_unlock_pair(struct tty_struct *tty,
					     struct tty_struct *tty2)
{
	__tty_ldisc_unlock(tty);
	if (tty2)
		__tty_ldisc_unlock(tty2);
}