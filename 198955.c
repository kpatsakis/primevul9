static int acm_tty_ioctl(struct tty_struct *tty,
					unsigned int cmd, unsigned long arg)
{
	struct acm *acm = tty->driver_data;
	int rv = -ENOIOCTLCMD;

	switch (cmd) {
	case TIOCMIWAIT:
		rv = usb_autopm_get_interface(acm->control);
		if (rv < 0) {
			rv = -EIO;
			break;
		}
		rv = wait_serial_change(acm, arg);
		usb_autopm_put_interface(acm->control);
		break;
	}

	return rv;
}