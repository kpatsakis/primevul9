static int pcan_usb_pro_handle_error(struct pcan_usb_pro_interface *usb_if,
				     struct pcan_usb_pro_rxstatus *er)
{
	const u16 raw_status = le16_to_cpu(er->status);
	const unsigned int ctrl_idx = (er->channel >> 4) & 0x0f;
	struct peak_usb_device *dev = usb_if->dev[ctrl_idx];
	struct net_device *netdev = dev->netdev;
	struct can_frame *can_frame;
	enum can_state new_state = CAN_STATE_ERROR_ACTIVE;
	u8 err_mask = 0;
	struct sk_buff *skb;
	struct skb_shared_hwtstamps *hwts;

	/* nothing should be sent while in BUS_OFF state */
	if (dev->can.state == CAN_STATE_BUS_OFF)
		return 0;

	if (!raw_status) {
		/* no error bit (back to active state) */
		dev->can.state = CAN_STATE_ERROR_ACTIVE;
		return 0;
	}

	if (raw_status & (PCAN_USBPRO_STATUS_OVERRUN |
			  PCAN_USBPRO_STATUS_QOVERRUN)) {
		/* trick to bypass next comparison and process other errors */
		new_state = CAN_STATE_MAX;
	}

	if (raw_status & PCAN_USBPRO_STATUS_BUS) {
		new_state = CAN_STATE_BUS_OFF;
	} else if (raw_status & PCAN_USBPRO_STATUS_ERROR) {
		u32 rx_err_cnt = (le32_to_cpu(er->err_frm) & 0x00ff0000) >> 16;
		u32 tx_err_cnt = (le32_to_cpu(er->err_frm) & 0xff000000) >> 24;

		if (rx_err_cnt > 127)
			err_mask |= CAN_ERR_CRTL_RX_PASSIVE;
		else if (rx_err_cnt > 96)
			err_mask |= CAN_ERR_CRTL_RX_WARNING;

		if (tx_err_cnt > 127)
			err_mask |= CAN_ERR_CRTL_TX_PASSIVE;
		else if (tx_err_cnt > 96)
			err_mask |= CAN_ERR_CRTL_TX_WARNING;

		if (err_mask & (CAN_ERR_CRTL_RX_WARNING |
				CAN_ERR_CRTL_TX_WARNING))
			new_state = CAN_STATE_ERROR_WARNING;
		else if (err_mask & (CAN_ERR_CRTL_RX_PASSIVE |
				     CAN_ERR_CRTL_TX_PASSIVE))
			new_state = CAN_STATE_ERROR_PASSIVE;
	}

	/* donot post any error if current state didn't change */
	if (dev->can.state == new_state)
		return 0;

	/* allocate an skb to store the error frame */
	skb = alloc_can_err_skb(netdev, &can_frame);
	if (!skb)
		return -ENOMEM;

	switch (new_state) {
	case CAN_STATE_BUS_OFF:
		can_frame->can_id |= CAN_ERR_BUSOFF;
		dev->can.can_stats.bus_off++;
		can_bus_off(netdev);
		break;

	case CAN_STATE_ERROR_PASSIVE:
		can_frame->can_id |= CAN_ERR_CRTL;
		can_frame->data[1] |= err_mask;
		dev->can.can_stats.error_passive++;
		break;

	case CAN_STATE_ERROR_WARNING:
		can_frame->can_id |= CAN_ERR_CRTL;
		can_frame->data[1] |= err_mask;
		dev->can.can_stats.error_warning++;
		break;

	case CAN_STATE_ERROR_ACTIVE:
		break;

	default:
		/* CAN_STATE_MAX (trick to handle other errors) */
		if (raw_status & PCAN_USBPRO_STATUS_OVERRUN) {
			can_frame->can_id |= CAN_ERR_PROT;
			can_frame->data[2] |= CAN_ERR_PROT_OVERLOAD;
			netdev->stats.rx_over_errors++;
			netdev->stats.rx_errors++;
		}

		if (raw_status & PCAN_USBPRO_STATUS_QOVERRUN) {
			can_frame->can_id |= CAN_ERR_CRTL;
			can_frame->data[1] |= CAN_ERR_CRTL_RX_OVERFLOW;
			netdev->stats.rx_over_errors++;
			netdev->stats.rx_errors++;
		}

		new_state = CAN_STATE_ERROR_ACTIVE;
		break;
	}

	dev->can.state = new_state;

	hwts = skb_hwtstamps(skb);
	peak_usb_get_ts_time(&usb_if->time_ref, le32_to_cpu(er->ts32), &hwts->hwtstamp);
	netdev->stats.rx_packets++;
	netdev->stats.rx_bytes += can_frame->can_dlc;
	netif_rx(skb);

	return 0;
}