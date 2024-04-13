static int pcan_usb_fd_send_cmd(struct peak_usb_device *dev, void *cmd_tail)
{
	void *cmd_head = pcan_usb_fd_cmd_buffer(dev);
	int err = 0;
	u8 *packet_ptr;
	int packet_len;
	ptrdiff_t cmd_len;

	/* usb device unregistered? */
	if (!(dev->state & PCAN_USB_STATE_CONNECTED))
		return 0;

	/* if a packet is not filled completely by commands, the command list
	 * is terminated with an "end of collection" record.
	 */
	cmd_len = cmd_tail - cmd_head;
	if (cmd_len <= (PCAN_UFD_CMD_BUFFER_SIZE - sizeof(u64))) {
		memset(cmd_tail, 0xff, sizeof(u64));
		cmd_len += sizeof(u64);
	}

	packet_ptr = cmd_head;
	packet_len = cmd_len;

	/* firmware is not able to re-assemble 512 bytes buffer in full-speed */
	if (unlikely(dev->udev->speed != USB_SPEED_HIGH))
		packet_len = min(packet_len, PCAN_UFD_LOSPD_PKT_SIZE);

	do {
		err = usb_bulk_msg(dev->udev,
				   usb_sndbulkpipe(dev->udev,
						   PCAN_USBPRO_EP_CMDOUT),
				   packet_ptr, packet_len,
				   NULL, PCAN_UFD_CMD_TIMEOUT_MS);
		if (err) {
			netdev_err(dev->netdev,
				   "sending command failure: %d\n", err);
			break;
		}

		packet_ptr += packet_len;
		cmd_len -= packet_len;

		if (cmd_len < PCAN_UFD_LOSPD_PKT_SIZE)
			packet_len = cmd_len;

	} while (packet_len > 0);

	return err;
}