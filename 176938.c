static void pcan_usb_pro_handle_ts(struct pcan_usb_pro_interface *usb_if,
				   struct pcan_usb_pro_rxts *ts)
{
	/* should wait until clock is stabilized */
	if (usb_if->cm_ignore_count > 0)
		usb_if->cm_ignore_count--;
	else
		peak_usb_set_ts_now(&usb_if->time_ref,
				    le32_to_cpu(ts->ts64[1]));
}