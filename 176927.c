static int pcan_usb_pro_encode_msg(struct peak_usb_device *dev,
				   struct sk_buff *skb, u8 *obuf, size_t *size)
{
	struct can_frame *cf = (struct can_frame *)skb->data;
	u8 data_type, len, flags;
	struct pcan_usb_pro_msg usb_msg;

	pcan_msg_init_empty(&usb_msg, obuf, *size);

	if ((cf->can_id & CAN_RTR_FLAG) || (cf->can_dlc == 0))
		data_type = PCAN_USBPRO_TXMSG0;
	else if (cf->can_dlc <= 4)
		data_type = PCAN_USBPRO_TXMSG4;
	else
		data_type = PCAN_USBPRO_TXMSG8;

	len = (dev->ctrl_idx << 4) | (cf->can_dlc & 0x0f);

	flags = 0;
	if (cf->can_id & CAN_EFF_FLAG)
		flags |= 0x02;
	if (cf->can_id & CAN_RTR_FLAG)
		flags |= 0x01;

	pcan_msg_add_rec(&usb_msg, data_type, 0, flags, len, cf->can_id,
			 cf->data);

	*size = usb_msg.rec_buffer_len;

	return 0;
}