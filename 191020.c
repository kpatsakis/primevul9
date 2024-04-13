static NTSTATUS read_packet_remainder(int fd, char *buffer,
				      unsigned int timeout, ssize_t len)
{
	if (len <= 0) {
		return NT_STATUS_OK;
	}

	return read_socket_with_timeout(fd, buffer, len, len, timeout, NULL);
}