static int acm_tty_install(struct tty_driver *driver, struct tty_struct *tty)
{
	struct acm *acm;
	int retval;

	acm = acm_get_by_minor(tty->index);
	if (!acm)
		return -ENODEV;

	retval = tty_standard_install(driver, tty);
	if (retval)
		goto error_init_termios;

	/*
	 * Suppress initial echoing for some devices which might send data
	 * immediately after acm driver has been installed.
	 */
	if (acm->quirks & DISABLE_ECHO)
		tty->termios.c_lflag &= ~ECHO;

	tty->driver_data = acm;

	return 0;

error_init_termios:
	tty_port_put(&acm->port);
	return retval;
}