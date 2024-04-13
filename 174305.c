seamless_line_handler(const char *line, void *data)
{
	if (!seamless_process_line(line, data))
	{
		warning("SeamlessRDP: Invalid request:%s\n", line);
	}
	return True;
}