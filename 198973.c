static int acm_tty_tiocmset(struct tty_struct *tty,
			    unsigned int set, unsigned int clear)
{
	struct acm *acm = tty->driver_data;
	unsigned int newctrl;

	newctrl = acm->ctrlout;
	set = (set & TIOCM_DTR ? ACM_CTRL_DTR : 0) |
					(set & TIOCM_RTS ? ACM_CTRL_RTS : 0);
	clear = (clear & TIOCM_DTR ? ACM_CTRL_DTR : 0) |
					(clear & TIOCM_RTS ? ACM_CTRL_RTS : 0);

	newctrl = (newctrl & ~clear) | set;

	if (acm->ctrlout == newctrl)
		return 0;
	return acm_set_control(acm, acm->ctrlout = newctrl);
}