static int pcan_usb_fd_set_bittiming_slow(struct peak_usb_device *dev,
					  struct can_bittiming *bt)
{
	struct pucan_timing_slow *cmd = pcan_usb_fd_cmd_buffer(dev);

	cmd->opcode_channel = pucan_cmd_opcode_channel(dev->ctrl_idx,
						       PUCAN_CMD_TIMING_SLOW);
	cmd->sjw_t = PUCAN_TSLOW_SJW_T(bt->sjw - 1,
				dev->can.ctrlmode & CAN_CTRLMODE_3_SAMPLES);

	cmd->tseg2 = PUCAN_TSLOW_TSEG2(bt->phase_seg2 - 1);
	cmd->tseg1 = PUCAN_TSLOW_TSEG1(bt->prop_seg + bt->phase_seg1 - 1);
	cmd->brp = cpu_to_le16(PUCAN_TSLOW_BRP(bt->brp - 1));

	cmd->ewl = 96;	/* default */

	/* send the command */
	return pcan_usb_fd_send_cmd(dev, ++cmd);
}