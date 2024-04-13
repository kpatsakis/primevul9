static void acm_tty_cleanup(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;

	tty_port_put(&acm->port);
}