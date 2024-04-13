static int sockent_server_listen (sockent_t *se) /* {{{ */
{
	struct addrinfo  ai_hints;
	struct addrinfo *ai_list, *ai_ptr;
	int              status;

        const char *node;
        const char *service;

	if (se == NULL)
		return (-1);

	assert (se->data.server.fd == NULL);
	assert (se->data.server.fd_num == 0);

        node = se->node;
        service = se->service;

        if (service == NULL)
          service = NET_DEFAULT_PORT;

        DEBUG ("network plugin: sockent_server_listen: node = %s; service = %s;",
            node, service);

	memset (&ai_hints, 0, sizeof (ai_hints));
	ai_hints.ai_flags  = 0;
#ifdef AI_PASSIVE
	ai_hints.ai_flags |= AI_PASSIVE;
#endif
#ifdef AI_ADDRCONFIG
	ai_hints.ai_flags |= AI_ADDRCONFIG;
#endif
	ai_hints.ai_family   = AF_UNSPEC;
	ai_hints.ai_socktype = SOCK_DGRAM;
	ai_hints.ai_protocol = IPPROTO_UDP;

	status = getaddrinfo (node, service, &ai_hints, &ai_list);
	if (status != 0)
	{
		ERROR ("network plugin: getaddrinfo (%s, %s) failed: %s",
				(se->node == NULL) ? "(null)" : se->node,
				(se->service == NULL) ? "(null)" : se->service,
				gai_strerror (status));
		return (-1);
	}

	for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next)
	{
		int *tmp;

		tmp = realloc (se->data.server.fd,
				sizeof (*tmp) * (se->data.server.fd_num + 1));
		if (tmp == NULL)
		{
			ERROR ("network plugin: realloc failed.");
			continue;
		}
		se->data.server.fd = tmp;
		tmp = se->data.server.fd + se->data.server.fd_num;

		*tmp = socket (ai_ptr->ai_family, ai_ptr->ai_socktype,
				ai_ptr->ai_protocol);
		if (*tmp < 0)
		{
			char errbuf[1024];
			ERROR ("network plugin: socket(2) failed: %s",
					sstrerror (errno, errbuf,
						sizeof (errbuf)));
			continue;
		}

		status = network_bind_socket (*tmp, ai_ptr, se->interface);
		if (status != 0)
		{
			close (*tmp);
			*tmp = -1;
			continue;
		}

		se->data.server.fd_num++;
		continue;
	} /* for (ai_list) */

	freeaddrinfo (ai_list);

	if (se->data.server.fd_num <= 0)
		return (-1);
	return (0);
} /* }}} int sockent_server_listen */