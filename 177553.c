isdn_all_eaz(int di, int ch)
{
	isdn_ctrl cmd;

	if (di < 0)
		return;
	cmd.driver = di;
	cmd.arg = ch;
	cmd.command = ISDN_CMD_SETEAZ;
	cmd.parm.num[0] = '\0';
	isdn_command(&cmd);
}