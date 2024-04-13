static int pcan_usb_fd_set_options(struct peak_usb_device *dev,
				   bool onoff, u16 ucan_mask, u16 usb_mask)
{
	struct pcan_ufd_options *cmd = pcan_usb_fd_cmd_buffer(dev);

	cmd->opcode_channel = pucan_cmd_opcode_channel(dev->ctrl_idx,
					(onoff) ? PUCAN_CMD_SET_EN_OPTION :
						  PUCAN_CMD_CLR_DIS_OPTION);

	cmd->ucan_mask = cpu_to_le16(ucan_mask);
	cmd->usb_mask = cpu_to_le16(usb_mask);

	/* send the command */
	return pcan_usb_fd_send_cmd(dev, ++cmd);
}