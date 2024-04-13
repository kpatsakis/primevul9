static int ems_usb_control_cmd(struct ems_usb *dev, u8 val)
{
	struct ems_cpc_msg cmd;

	cmd.type = CPC_CMD_TYPE_CONTROL;
	cmd.length = CPC_MSG_HEADER_LEN + 1;

	cmd.msgid = 0;

	cmd.msg.generic[0] = val;

	return ems_usb_command_msg(dev, &cmd);
}