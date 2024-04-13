static int get_serial_info(struct tty_struct *tty, struct serial_struct *ss)
{
	struct acm *acm = tty->driver_data;

	ss->xmit_fifo_size = acm->writesize;
	ss->baud_base = le32_to_cpu(acm->line.dwDTERate);
	ss->close_delay	= acm->port.close_delay / 10;
	ss->closing_wait = acm->port.closing_wait == ASYNC_CLOSING_WAIT_NONE ?
				ASYNC_CLOSING_WAIT_NONE :
				acm->port.closing_wait / 10;
	return 0;
}