pipe_cleanup (Pipe *pipe)
{
	if (pipe->channel) {
		g_source_remove (pipe->watch);
		pipe->watch = 0;
		g_io_channel_shutdown (pipe->channel, FALSE, NULL);
		g_io_channel_unref (pipe->channel);
		pipe->channel = NULL;
	}
	if (pipe->fd >= 0) {
		close (pipe->fd);
		pipe->fd = -1;
	}
	if (pipe->buf) {
		g_string_free (pipe->buf, TRUE);
		pipe->buf = NULL;
	}
}