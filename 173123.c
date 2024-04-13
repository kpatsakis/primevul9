static void bnx2x_set_requested_fc(struct bnx2x *bp)
{
	/* Initialize link parameters structure variables
	 * It is recommended to turn off RX FC for jumbo frames
	 *  for better performance
	 */
	if (CHIP_IS_E1x(bp) && (bp->dev->mtu > 5000))
		bp->link_params.req_fc_auto_adv = BNX2X_FLOW_CTRL_TX;
	else
		bp->link_params.req_fc_auto_adv = BNX2X_FLOW_CTRL_BOTH;
}