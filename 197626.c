static void pcan_usb_fd_decode_ts(struct pcan_usb_fd_if *usb_if,
				  struct pucan_msg *rx_msg)
{
	struct pcan_ufd_ts_msg *ts = (struct pcan_ufd_ts_msg *)rx_msg;

	/* should wait until clock is stabilized */
	if (usb_if->cm_ignore_count > 0)
		usb_if->cm_ignore_count--;
	else
		peak_usb_set_ts_now(&usb_if->time_ref, le32_to_cpu(ts->ts_low));
}