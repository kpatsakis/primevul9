static void acm_tty_close(struct tty_struct *tty, struct file *filp)
{
	struct acm *acm = tty->driver_data;

	tty_port_close(&acm->port, tty, filp);
}