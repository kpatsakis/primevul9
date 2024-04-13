struct tty_ldisc *tty_ldisc_ref_wait(struct tty_struct *tty)
{
	ldsem_down_read(&tty->ldisc_sem, MAX_SCHEDULE_TIMEOUT);
	WARN_ON(!tty->ldisc);
	return tty->ldisc;
}