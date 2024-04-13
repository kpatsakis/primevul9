static void tty_ldisc_kill(struct tty_struct *tty)
{
	/*
	 * Now kill off the ldisc
	 */
	tty_ldisc_close(tty, tty->ldisc);
	tty_ldisc_put(tty->ldisc);
	/* Force an oops if we mess this up */
	tty->ldisc = NULL;

	/* Ensure the next open requests the N_TTY ldisc */
	tty_set_termios_ldisc(tty, N_TTY);
}