void tty_ldisc_release(struct tty_struct *tty)
{
	struct tty_struct *o_tty = tty->link;

	/*
	 * Shutdown this line discipline. As this is the final close,
	 * it does not race with the set_ldisc code path.
	 */

	tty_ldisc_lock_pair(tty, o_tty);
	tty_ldisc_kill(tty);
	if (o_tty)
		tty_ldisc_kill(o_tty);
	tty_ldisc_unlock_pair(tty, o_tty);

	/* And the memory resources remaining (buffers, termios) will be
	   disposed of when the kref hits zero */

	tty_ldisc_debug(tty, "released\n");
}