static int fe_send_command(struct dvb_frontend* fe, const u8 command,
			   int param_length, const u8 params[],
			   int *result_length, u8 cmd_result[])
{
	struct ttusb_dec* dec = fe->dvb->priv;
	return ttusb_dec_send_command(dec, command, param_length, params, result_length, cmd_result);
}