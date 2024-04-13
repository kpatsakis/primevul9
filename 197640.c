static int pcan_usb_fd_set_can_led(struct peak_usb_device *dev, u8 led_mode)
{
	struct pcan_ufd_led *cmd = pcan_usb_fd_cmd_buffer(dev);

	cmd->opcode_channel = pucan_cmd_opcode_channel(dev->ctrl_idx,
						       PCAN_UFD_CMD_LED_SET);
	cmd->mode = led_mode;

	/* send the command */
	return pcan_usb_fd_send_cmd(dev, ++cmd);
}