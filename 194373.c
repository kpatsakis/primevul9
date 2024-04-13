void tty_ldisc_hangup(struct tty_struct *tty)
{
	struct tty_ldisc *ld;
	int reset = tty->driver->flags & TTY_DRIVER_RESET_TERMIOS;
	int err = 0;

	tty_ldisc_debug(tty, "%p: closing\n", tty->ldisc);

	ld = tty_ldisc_ref(tty);
	if (ld != NULL) {
		if (ld->ops->flush_buffer)
			ld->ops->flush_buffer(tty);
		tty_driver_flush_buffer(tty);
		if ((test_bit(TTY_DO_WRITE_WAKEUP, &tty->flags)) &&
		    ld->ops->write_wakeup)
			ld->ops->write_wakeup(tty);
		if (ld->ops->hangup)
			ld->ops->hangup(tty);
		tty_ldisc_deref(ld);
	}

	wake_up_interruptible_poll(&tty->write_wait, POLLOUT);
	wake_up_interruptible_poll(&tty->read_wait, POLLIN);

	/*
	 * Shutdown the current line discipline, and reset it to
	 * N_TTY if need be.
	 *
	 * Avoid racing set_ldisc or tty_ldisc_release
	 */
	tty_ldisc_lock(tty, MAX_SCHEDULE_TIMEOUT);

	if (tty->ldisc) {

		/* At this point we have a halted ldisc; we want to close it and
		   reopen a new ldisc. We could defer the reopen to the next
		   open but it means auditing a lot of other paths so this is
		   a FIXME */
		if (reset == 0) {

			if (!tty_ldisc_reinit(tty, tty->termios.c_line))
				err = tty_ldisc_open(tty, tty->ldisc);
			else
				err = 1;
		}
		/* If the re-open fails or we reset then go to N_TTY. The
		   N_TTY open cannot fail */
		if (reset || err) {
			BUG_ON(tty_ldisc_reinit(tty, N_TTY));
			WARN_ON(tty_ldisc_open(tty, tty->ldisc));
		}
	}
	tty_ldisc_unlock(tty);
	if (reset)
		tty_reset_termios(tty);

	tty_ldisc_debug(tty, "%p: re-opened\n", tty->ldisc);
}