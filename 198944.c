static void acm_tty_hangup(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;

	tty_port_hangup(&acm->port);
}