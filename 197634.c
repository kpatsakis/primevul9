static int pcan_usb_fd_set_bus(struct peak_usb_device *dev, u8 onoff)
{
	u8 *pc = pcan_usb_fd_cmd_buffer(dev);
	int l;

	if (onoff) {
		/* build the cmds list to enter operational mode */
		l = pcan_usb_fd_build_restart_cmd(dev, pc);
	} else {
		struct pucan_command *cmd = (struct pucan_command *)pc;

		/* build cmd to go back to reset mode */
		cmd->opcode_channel = pucan_cmd_opcode_channel(dev->ctrl_idx,
							PUCAN_CMD_RESET_MODE);
		l = sizeof(struct pucan_command);
	}

	/* send the command */
	return pcan_usb_fd_send_cmd(dev, pc + l);
}