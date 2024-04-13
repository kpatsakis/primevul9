static int acm_tty_open(struct tty_struct *tty, struct file *filp)
{
	struct acm *acm = tty->driver_data;

	return tty_port_open(&acm->port, tty, filp);
}