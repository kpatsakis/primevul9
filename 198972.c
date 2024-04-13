static int acm_tty_break_ctl(struct tty_struct *tty, int state)
{
	struct acm *acm = tty->driver_data;
	int retval;

	retval = acm_send_break(acm, state ? 0xffff : 0);
	if (retval < 0)
		dev_dbg(&acm->control->dev,
			"%s - send break failed\n", __func__);
	return retval;
}