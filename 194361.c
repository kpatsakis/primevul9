static int tty_ldisc_reinit(struct tty_struct *tty, int ldisc)
{
	struct tty_ldisc *ld = tty_ldisc_get(tty, ldisc);

	if (IS_ERR(ld))
		return -1;

	tty_ldisc_close(tty, tty->ldisc);
	tty_ldisc_put(tty->ldisc);
	/*
	 *	Switch the line discipline back
	 */
	tty->ldisc = ld;
	tty_set_termios_ldisc(tty, ldisc);

	return 0;
}