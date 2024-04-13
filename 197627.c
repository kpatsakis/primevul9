static int pcan_usb_fd_build_restart_cmd(struct peak_usb_device *dev, u8 *buf)
{
	struct pucan_wr_err_cnt *prc;
	struct pucan_command *cmd;
	u8 *pc = buf;

	/* 1st, reset error counters: */
	prc = (struct pucan_wr_err_cnt *)pc;
	prc->opcode_channel = pucan_cmd_opcode_channel(dev->ctrl_idx,
						       PUCAN_CMD_WR_ERR_CNT);

	/* select both counters */
	prc->sel_mask = cpu_to_le16(PUCAN_WRERRCNT_TE|PUCAN_WRERRCNT_RE);

	/* and reset their values */
	prc->tx_counter = 0;
	prc->rx_counter = 0;

	/* moves the pointer forward */
	pc += sizeof(struct pucan_wr_err_cnt);

	/* add command to switch from ISO to non-ISO mode, if fw allows it */
	if (dev->can.ctrlmode_supported & CAN_CTRLMODE_FD_NON_ISO) {
		struct pucan_options *puo = (struct pucan_options *)pc;

		puo->opcode_channel =
			(dev->can.ctrlmode & CAN_CTRLMODE_FD_NON_ISO) ?
			pucan_cmd_opcode_channel(dev->ctrl_idx,
						 PUCAN_CMD_CLR_DIS_OPTION) :
			pucan_cmd_opcode_channel(dev->ctrl_idx,
						 PUCAN_CMD_SET_EN_OPTION);

		puo->options = cpu_to_le16(PUCAN_OPTION_CANDFDISO);

		/* to be sure that no other extended bits will be taken into
		 * account
		 */
		puo->unused = 0;

		/* moves the pointer forward */
		pc += sizeof(struct pucan_options);
	}

	/* next, go back to operational mode */
	cmd = (struct pucan_command *)pc;
	cmd->opcode_channel = pucan_cmd_opcode_channel(dev->ctrl_idx,
				(dev->can.ctrlmode & CAN_CTRLMODE_LISTENONLY) ?
						PUCAN_CMD_LISTEN_ONLY_MODE :
						PUCAN_CMD_NORMAL_MODE);
	pc += sizeof(struct pucan_command);

	return pc - buf;
}