static void acm_tty_unthrottle(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;

	clear_bit(ACM_THROTTLED, &acm->flags);

	/* Matches the smp_mb__after_atomic() in acm_read_bulk_callback(). */
	smp_mb();

	acm_submit_read_urbs(acm, GFP_KERNEL);
}