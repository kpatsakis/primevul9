tty_ldisc_lock(struct tty_struct *tty, unsigned long timeout)
{
	int ret;

	ret = __tty_ldisc_lock(tty, timeout);
	if (!ret)
		return -EBUSY;
	set_bit(TTY_LDISC_HALTED, &tty->flags);
	return 0;
}