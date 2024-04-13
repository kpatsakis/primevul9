static int pcan_usb_fd_set_clock_domain(struct peak_usb_device *dev,
					u8 clk_mode)
{
	struct pcan_ufd_clock *cmd = pcan_usb_fd_cmd_buffer(dev);

	cmd->opcode_channel = pucan_cmd_opcode_channel(dev->ctrl_idx,
						       PCAN_UFD_CMD_CLK_SET);
	cmd->mode = clk_mode;

	/* send the command */
	return pcan_usb_fd_send_cmd(dev, ++cmd);
}