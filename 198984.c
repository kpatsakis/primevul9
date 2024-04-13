static int acm_tty_chars_in_buffer(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;
	/*
	 * if the device was unplugged then any remaining characters fell out
	 * of the connector ;)
	 */
	if (acm->disconnected)
		return 0;
	/*
	 * This is inaccurate (overcounts), but it works.
	 */
	return (ACM_NW - acm_wb_is_avail(acm)) * acm->writesize;
}