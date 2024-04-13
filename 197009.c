static void ems_usb_rx_can_msg(struct ems_usb *dev, struct ems_cpc_msg *msg)
{
	struct can_frame *cf;
	struct sk_buff *skb;
	int i;
	struct net_device_stats *stats = &dev->netdev->stats;

	skb = alloc_can_skb(dev->netdev, &cf);
	if (skb == NULL)
		return;

	cf->can_id = le32_to_cpu(msg->msg.can_msg.id);
	cf->len = can_cc_dlc2len(msg->msg.can_msg.length & 0xF);

	if (msg->type == CPC_MSG_TYPE_EXT_CAN_FRAME ||
	    msg->type == CPC_MSG_TYPE_EXT_RTR_FRAME)
		cf->can_id |= CAN_EFF_FLAG;

	if (msg->type == CPC_MSG_TYPE_RTR_FRAME ||
	    msg->type == CPC_MSG_TYPE_EXT_RTR_FRAME) {
		cf->can_id |= CAN_RTR_FLAG;
	} else {
		for (i = 0; i < cf->len; i++)
			cf->data[i] = msg->msg.can_msg.msg[i];

		stats->rx_bytes += cf->len;
	}
	stats->rx_packets++;

	netif_rx(skb);
}