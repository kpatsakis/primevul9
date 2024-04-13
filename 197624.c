static int pcan_usb_fd_set_bittiming_fast(struct peak_usb_device *dev,
					  struct can_bittiming *bt)
{
	struct pucan_timing_fast *cmd = pcan_usb_fd_cmd_buffer(dev);

	cmd->opcode_channel = pucan_cmd_opcode_channel(dev->ctrl_idx,
						       PUCAN_CMD_TIMING_FAST);
	cmd->sjw = PUCAN_TFAST_SJW(bt->sjw - 1);
	cmd->tseg2 = PUCAN_TFAST_TSEG2(bt->phase_seg2 - 1);
	cmd->tseg1 = PUCAN_TFAST_TSEG1(bt->prop_seg + bt->phase_seg1 - 1);
	cmd->brp = cpu_to_le16(PUCAN_TFAST_BRP(bt->brp - 1));

	/* send the command */
	return pcan_usb_fd_send_cmd(dev, ++cmd);
}