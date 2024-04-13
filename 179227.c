void rndis_add_hdr(struct sk_buff *skb)
{
	struct rndis_packet_msg_type *header;

	if (!skb)
		return;
	header = skb_push(skb, sizeof(*header));
	memset(header, 0, sizeof *header);
	header->MessageType = cpu_to_le32(RNDIS_MSG_PACKET);
	header->MessageLength = cpu_to_le32(skb->len);
	header->DataOffset = cpu_to_le32(36);
	header->DataLength = cpu_to_le32(skb->len - sizeof(*header));
}