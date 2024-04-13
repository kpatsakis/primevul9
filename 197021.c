static int ems_usb_command_msg(struct ems_usb *dev, struct ems_cpc_msg *msg)
{
	int actual_length;

	/* Copy payload */
	memcpy(&dev->tx_msg_buffer[CPC_HEADER_SIZE], msg,
	       msg->length + CPC_MSG_HEADER_LEN);

	/* Clear header */
	memset(&dev->tx_msg_buffer[0], 0, CPC_HEADER_SIZE);

	return usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, 2),
			    &dev->tx_msg_buffer[0],
			    msg->length + CPC_MSG_HEADER_LEN + CPC_HEADER_SIZE,
			    &actual_length, 1000);
}