camel_imapx_server_ref_current_command (CamelIMAPXServer *is)
{
	CamelIMAPXCommand *command;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);

	COMMAND_LOCK (is);

	command = is->priv->current_command;
	if (command)
		camel_imapx_command_ref (command);

	COMMAND_UNLOCK (is);

	return command;
}