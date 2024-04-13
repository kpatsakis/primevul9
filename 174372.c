lspci_process_line(const char *line, void *data)
{
	char *lspci_command[5] = { "lspci", "-m", "-n", "-v", NULL };

	if (!strcmp(line, "LSPCI"))
	{
		memset(&current_device, 0, sizeof(current_device));
		subprocess(lspci_command, handle_child_line, NULL);
		/* Send single dot to indicate end of enumeration */
		lspci_send(".\n");
	}
	else
	{
		error("lspci protocol error: Invalid line '%s'\n", line);
	}
	return True;
}