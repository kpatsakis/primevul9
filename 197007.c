static void ems_usb_rx_err(struct ems_usb *dev, struct ems_cpc_msg *msg)
{
	struct can_frame *cf;
	struct sk_buff *skb;
	struct net_device_stats *stats = &dev->netdev->stats;

	skb = alloc_can_err_skb(dev->netdev, &cf);
	if (skb == NULL)
		return;

	if (msg->type == CPC_MSG_TYPE_CAN_STATE) {
		u8 state = msg->msg.can_state;

		if (state & SJA1000_SR_BS) {
			dev->can.state = CAN_STATE_BUS_OFF;
			cf->can_id |= CAN_ERR_BUSOFF;

			dev->can.can_stats.bus_off++;
			can_bus_off(dev->netdev);
		} else if (state & SJA1000_SR_ES) {
			dev->can.state = CAN_STATE_ERROR_WARNING;
			dev->can.can_stats.error_warning++;
		} else {
			dev->can.state = CAN_STATE_ERROR_ACTIVE;
			dev->can.can_stats.error_passive++;
		}
	} else if (msg->type == CPC_MSG_TYPE_CAN_FRAME_ERROR) {
		u8 ecc = msg->msg.error.cc.regs.sja1000.ecc;
		u8 txerr = msg->msg.error.cc.regs.sja1000.txerr;
		u8 rxerr = msg->msg.error.cc.regs.sja1000.rxerr;

		/* bus error interrupt */
		dev->can.can_stats.bus_error++;
		stats->rx_errors++;

		cf->can_id |= CAN_ERR_PROT | CAN_ERR_BUSERROR;

		switch (ecc & SJA1000_ECC_MASK) {
		case SJA1000_ECC_BIT:
			cf->data[2] |= CAN_ERR_PROT_BIT;
			break;
		case SJA1000_ECC_FORM:
			cf->data[2] |= CAN_ERR_PROT_FORM;
			break;
		case SJA1000_ECC_STUFF:
			cf->data[2] |= CAN_ERR_PROT_STUFF;
			break;
		default:
			cf->data[3] = ecc & SJA1000_ECC_SEG;
			break;
		}

		/* Error occurred during transmission? */
		if ((ecc & SJA1000_ECC_DIR) == 0)
			cf->data[2] |= CAN_ERR_PROT_TX;

		if (dev->can.state == CAN_STATE_ERROR_WARNING ||
		    dev->can.state == CAN_STATE_ERROR_PASSIVE) {
			cf->can_id |= CAN_ERR_CRTL;
			cf->data[1] = (txerr > rxerr) ?
			    CAN_ERR_CRTL_TX_PASSIVE : CAN_ERR_CRTL_RX_PASSIVE;
		}
	} else if (msg->type == CPC_MSG_TYPE_OVERRUN) {
		cf->can_id |= CAN_ERR_CRTL;
		cf->data[1] = CAN_ERR_CRTL_RX_OVERFLOW;

		stats->rx_over_errors++;
		stats->rx_errors++;
	}

	netif_rx(skb);
}