static void acm_process_read_urb(struct acm *acm, struct urb *urb)
{
	if (!urb->actual_length)
		return;

	tty_insert_flip_string(&acm->port, urb->transfer_buffer,
			urb->actual_length);
	tty_flip_buffer_push(&acm->port);
}