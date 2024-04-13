static u16 xemaclite_recv_data(struct net_local *drvdata, u8 *data, int maxlen)
{
	void __iomem *addr;
	u16 length, proto_type;
	u32 reg_data;

	/* Determine the expected buffer address */
	addr = (drvdata->base_addr + drvdata->next_rx_buf_to_use);

	/* Verify which buffer has valid data */
	reg_data = xemaclite_readl(addr + XEL_RSR_OFFSET);

	if ((reg_data & XEL_RSR_RECV_DONE_MASK) == XEL_RSR_RECV_DONE_MASK) {
		if (drvdata->rx_ping_pong != 0)
			drvdata->next_rx_buf_to_use ^= XEL_BUFFER_OFFSET;
	} else {
		/* The instance is out of sync, try other buffer if other
		 * buffer is configured, return 0 otherwise. If the instance is
		 * out of sync, do not update the 'next_rx_buf_to_use' since it
		 * will correct on subsequent calls
		 */
		if (drvdata->rx_ping_pong != 0)
			addr = (void __iomem __force *)
				((uintptr_t __force)addr ^
				 XEL_BUFFER_OFFSET);
		else
			return 0;	/* No data was available */

		/* Verify that buffer has valid data */
		reg_data = xemaclite_readl(addr + XEL_RSR_OFFSET);
		if ((reg_data & XEL_RSR_RECV_DONE_MASK) !=
		     XEL_RSR_RECV_DONE_MASK)
			return 0;	/* No data was available */
	}

	/* Get the protocol type of the ethernet frame that arrived
	 */
	proto_type = ((ntohl(xemaclite_readl(addr + XEL_HEADER_OFFSET +
			XEL_RXBUFF_OFFSET)) >> XEL_HEADER_SHIFT) &
			XEL_RPLR_LENGTH_MASK);

	/* Check if received ethernet frame is a raw ethernet frame
	 * or an IP packet or an ARP packet
	 */
	if (proto_type > ETH_DATA_LEN) {

		if (proto_type == ETH_P_IP) {
			length = ((ntohl(xemaclite_readl(addr +
					XEL_HEADER_IP_LENGTH_OFFSET +
					XEL_RXBUFF_OFFSET)) >>
					XEL_HEADER_SHIFT) &
					XEL_RPLR_LENGTH_MASK);
			length = min_t(u16, length, ETH_DATA_LEN);
			length += ETH_HLEN + ETH_FCS_LEN;

		} else if (proto_type == ETH_P_ARP)
			length = XEL_ARP_PACKET_SIZE + ETH_HLEN + ETH_FCS_LEN;
		else
			/* Field contains type other than IP or ARP, use max
			 * frame size and let user parse it
			 */
			length = ETH_FRAME_LEN + ETH_FCS_LEN;
	} else
		/* Use the length in the frame, plus the header and trailer */
		length = proto_type + ETH_HLEN + ETH_FCS_LEN;

	if (WARN_ON(length > maxlen))
		length = maxlen;

	/* Read from the EmacLite device */
	xemaclite_aligned_read((u32 __force *)(addr + XEL_RXBUFF_OFFSET),
				data, length);

	/* Acknowledge the frame */
	reg_data = xemaclite_readl(addr + XEL_RSR_OFFSET);
	reg_data &= ~XEL_RSR_RECV_DONE_MASK;
	xemaclite_writel(reg_data, addr + XEL_RSR_OFFSET);

	return length;
}