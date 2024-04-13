void tty_ldisc_init(struct tty_struct *tty)
{
	struct tty_ldisc *ld = tty_ldisc_get(tty, N_TTY);
	if (IS_ERR(ld))
		panic("n_tty: init_tty");
	tty->ldisc = ld;
}