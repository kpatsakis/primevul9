pipe_echo_finish (Pipe *pipe)
{
	GIOStatus status;
	gsize bytes_read;
	char buf[512];

	do {
		bytes_read = 0;
		status = g_io_channel_read_chars (pipe->channel,
		                                  buf,
		                                  sizeof (buf),
		                                  &bytes_read,
		                                  NULL);
		if (bytes_read) {
			fprintf (pipe->logf, "%.*s", (int) bytes_read, buf);
			fflush (pipe->logf);
		}
	} while (status == G_IO_STATUS_NORMAL);
}