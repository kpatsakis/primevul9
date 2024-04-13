static void tty_ldisc_unlock(struct tty_struct *tty)
{
	clear_bit(TTY_LDISC_HALTED, &tty->flags);
	__tty_ldisc_unlock(tty);
}