void tty_ldisc_deinit(struct tty_struct *tty)
{
	tty_ldisc_put(tty->ldisc);
	tty->ldisc = NULL;
}