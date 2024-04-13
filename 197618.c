static int pcan_usb_fd_encode_msg(struct peak_usb_device *dev,
				  struct sk_buff *skb, u8 *obuf, size_t *size)
{
	struct pucan_tx_msg *tx_msg = (struct pucan_tx_msg *)obuf;
	struct canfd_frame *cfd = (struct canfd_frame *)skb->data;
	u16 tx_msg_size, tx_msg_flags;
	u8 can_dlc;

	tx_msg_size = ALIGN(sizeof(struct pucan_tx_msg) + cfd->len, 4);
	tx_msg->size = cpu_to_le16(tx_msg_size);
	tx_msg->type = cpu_to_le16(PUCAN_MSG_CAN_TX);

	tx_msg_flags = 0;
	if (cfd->can_id & CAN_EFF_FLAG) {
		tx_msg_flags |= PUCAN_MSG_EXT_ID;
		tx_msg->can_id = cpu_to_le32(cfd->can_id & CAN_EFF_MASK);
	} else {
		tx_msg->can_id = cpu_to_le32(cfd->can_id & CAN_SFF_MASK);
	}

	if (can_is_canfd_skb(skb)) {
		/* considering a CANFD frame */
		can_dlc = can_len2dlc(cfd->len);

		tx_msg_flags |= PUCAN_MSG_EXT_DATA_LEN;

		if (cfd->flags & CANFD_BRS)
			tx_msg_flags |= PUCAN_MSG_BITRATE_SWITCH;

		if (cfd->flags & CANFD_ESI)
			tx_msg_flags |= PUCAN_MSG_ERROR_STATE_IND;
	} else {
		/* CAND 2.0 frames */
		can_dlc = cfd->len;

		if (cfd->can_id & CAN_RTR_FLAG)
			tx_msg_flags |= PUCAN_MSG_RTR;
	}

	tx_msg->flags = cpu_to_le16(tx_msg_flags);
	tx_msg->channel_dlc = PUCAN_MSG_CHANNEL_DLC(dev->ctrl_idx, can_dlc);
	memcpy(tx_msg->d, cfd->data, cfd->len);

	/* add null size message to tag the end (messages are 32-bits aligned)
	 */
	tx_msg = (struct pucan_tx_msg *)(obuf + tx_msg_size);

	tx_msg->size = 0;

	/* set the whole size of the USB packet to send */
	*size = tx_msg_size + sizeof(u32);

	return 0;
}