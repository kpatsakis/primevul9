static int tty_ldisc_open(struct tty_struct *tty, struct tty_ldisc *ld)
{
	WARN_ON(test_and_set_bit(TTY_LDISC_OPEN, &tty->flags));
	if (ld->ops->open) {
		int ret;
                /* BTM here locks versus a hangup event */
		ret = ld->ops->open(tty);
		if (ret)
			clear_bit(TTY_LDISC_OPEN, &tty->flags);

		tty_ldisc_debug(tty, "%p: opened\n", tty->ldisc);
		return ret;
	}
	return 0;
}