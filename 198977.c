static void acm_tty_throttle(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;

	set_bit(ACM_THROTTLED, &acm->flags);
}