static int pcan_usb_pro_wait_rsp(struct peak_usb_device *dev,
				 struct pcan_usb_pro_msg *pum)
{
	u8 req_data_type, req_channel;
	int actual_length;
	int i, err = 0;

	/* usb device unregistered? */
	if (!(dev->state & PCAN_USB_STATE_CONNECTED))
		return 0;

	req_data_type = pum->u.rec_buffer[4];
	req_channel = pum->u.rec_buffer[5];

	*pum->u.rec_cnt = 0;
	for (i = 0; !err && i < PCAN_USBPRO_RSP_SUBMIT_MAX; i++) {
		struct pcan_usb_pro_msg rsp;
		union pcan_usb_pro_rec *pr;
		u32 r, rec_cnt;
		u16 rec_len;
		u8 *pc;

		err = usb_bulk_msg(dev->udev,
			usb_rcvbulkpipe(dev->udev, PCAN_USBPRO_EP_CMDIN),
			pum->u.rec_buffer, pum->rec_buffer_len,
			&actual_length, PCAN_USBPRO_COMMAND_TIMEOUT);
		if (err) {
			netdev_err(dev->netdev, "waiting rsp error %d\n", err);
			break;
		}

		if (actual_length == 0)
			continue;

		err = -EBADMSG;
		if (actual_length < PCAN_USBPRO_MSG_HEADER_LEN) {
			netdev_err(dev->netdev,
				   "got abnormal too small rsp (len=%d)\n",
				   actual_length);
			break;
		}

		pc = pcan_msg_init(&rsp, pum->u.rec_buffer,
			actual_length);

		rec_cnt = le32_to_cpu(*rsp.u.rec_cnt);

		/* loop on records stored into message */
		for (r = 0; r < rec_cnt; r++) {
			pr = (union pcan_usb_pro_rec *)pc;
			rec_len = pcan_usb_pro_sizeof_rec[pr->data_type];
			if (!rec_len) {
				netdev_err(dev->netdev,
					   "got unprocessed record in msg\n");
				pcan_dump_mem("rcvd rsp msg", pum->u.rec_buffer,
					      actual_length);
				break;
			}

			/* check if response corresponds to request */
			if (pr->data_type != req_data_type)
				netdev_err(dev->netdev,
					   "got unwanted rsp %xh: ignored\n",
					   pr->data_type);

			/* check if channel in response corresponds too */
			else if ((req_channel != 0xff) && \
				(pr->bus_act.channel != req_channel))
				netdev_err(dev->netdev,
					"got rsp %xh but on chan%u: ignored\n",
					req_data_type, pr->bus_act.channel);

			/* got the response */
			else
				return 0;

			/* otherwise, go on with next record in message */
			pc += rec_len;
		}
	}

	return (i >= PCAN_USBPRO_RSP_SUBMIT_MAX) ? -ERANGE : err;
}