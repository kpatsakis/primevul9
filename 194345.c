void tty_ldisc_flush(struct tty_struct *tty)
{
	struct tty_ldisc *ld = tty_ldisc_ref(tty);

	tty_buffer_flush(tty, ld);
	if (ld)
		tty_ldisc_deref(ld);
}