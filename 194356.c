static void tty_reset_termios(struct tty_struct *tty)
{
	down_write(&tty->termios_rwsem);
	tty->termios = tty->driver->init_termios;
	tty->termios.c_ispeed = tty_termios_input_baud_rate(&tty->termios);
	tty->termios.c_ospeed = tty_termios_baud_rate(&tty->termios);
	up_write(&tty->termios_rwsem);
}