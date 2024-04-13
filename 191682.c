static void networt_send_buffer_plain (sockent_t *se, /* {{{ */
		const char *buffer, size_t buffer_size)
{
	int status;

	while (42)
	{
		status = sockent_client_connect (se);
		if (status != 0)
			return;

		status = sendto (se->data.client.fd, buffer, buffer_size,
				/* flags = */ 0,
				(struct sockaddr *) se->data.client.addr,
				se->data.client.addrlen);
		if (status < 0)
		{
			char errbuf[1024];

			if ((errno == EINTR) || (errno == EAGAIN))
				continue;

			ERROR ("network plugin: sendto failed: %s. Closing sending socket.",
					sstrerror (errno, errbuf, sizeof (errbuf)));
			sockent_client_disconnect (se);
			return;
		}

		break;
	} /* while (42) */
} /* }}} void networt_send_buffer_plain */