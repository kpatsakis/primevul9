pipe_setup (Pipe *pipe, FILE *logf, gpointer user_data)
{
	GIOFlags flags = 0;

	pipe->logf = logf;
	pipe->buf = g_string_sized_new (512);

	pipe->channel = g_io_channel_unix_new (pipe->fd);
	g_io_channel_set_encoding (pipe->channel, NULL, NULL);
	flags = g_io_channel_get_flags (pipe->channel);
	g_io_channel_set_flags (pipe->channel, flags | G_IO_FLAG_NONBLOCK, NULL);
	g_io_channel_set_buffered (pipe->channel, FALSE);

	pipe->watch = g_io_add_watch (pipe->channel,
	                              G_IO_IN | G_IO_ERR | G_IO_PRI,
	                              data_available,
	                              user_data);
}