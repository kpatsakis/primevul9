static void smbd_server_connection_handler(struct event_context *ev,
					   struct fd_event *fde,
					   uint16_t flags,
					   void *private_data)
{
	struct smbd_server_connection *conn = talloc_get_type(private_data,
					      struct smbd_server_connection);

	if (flags & EVENT_FD_WRITE) {
		smbd_server_connection_write_handler(conn);
	} else if (flags & EVENT_FD_READ) {
		smbd_server_connection_read_handler(conn);
	}
}