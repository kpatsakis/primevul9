static int xemaclite_send_data(struct net_local *drvdata, u8 *data,
			       unsigned int byte_count)
{
	u32 reg_data;
	void __iomem *addr;

	/* Determine the expected Tx buffer address */
	addr = drvdata->base_addr + drvdata->next_tx_buf_to_use;

	/* If the length is too large, truncate it */
	if (byte_count > ETH_FRAME_LEN)
		byte_count = ETH_FRAME_LEN;

	/* Check if the expected buffer is available */
	reg_data = xemaclite_readl(addr + XEL_TSR_OFFSET);
	if ((reg_data & (XEL_TSR_XMIT_BUSY_MASK |
	     XEL_TSR_XMIT_ACTIVE_MASK)) == 0) {

		/* Switch to next buffer if configured */
		if (drvdata->tx_ping_pong != 0)
			drvdata->next_tx_buf_to_use ^= XEL_BUFFER_OFFSET;
	} else if (drvdata->tx_ping_pong != 0) {
		/* If the expected buffer is full, try the other buffer,
		 * if it is configured in HW
		 */

		addr = (void __iomem __force *)((uintptr_t __force)addr ^
						 XEL_BUFFER_OFFSET);
		reg_data = xemaclite_readl(addr + XEL_TSR_OFFSET);

		if ((reg_data & (XEL_TSR_XMIT_BUSY_MASK |
		     XEL_TSR_XMIT_ACTIVE_MASK)) != 0)
			return -1; /* Buffers were full, return failure */
	} else
		return -1; /* Buffer was full, return failure */

	/* Write the frame to the buffer */
	xemaclite_aligned_write(data, (u32 __force *)addr, byte_count);

	xemaclite_writel((byte_count & XEL_TPLR_LENGTH_MASK),
			 addr + XEL_TPLR_OFFSET);

	/* Update the Tx Status Register to indicate that there is a
	 * frame to send. Set the XEL_TSR_XMIT_ACTIVE_MASK flag which
	 * is used by the interrupt handler to check whether a frame
	 * has been transmitted
	 */
	reg_data = xemaclite_readl(addr + XEL_TSR_OFFSET);
	reg_data |= (XEL_TSR_XMIT_BUSY_MASK | XEL_TSR_XMIT_ACTIVE_MASK);
	xemaclite_writel(reg_data, addr + XEL_TSR_OFFSET);

	return 0;
}