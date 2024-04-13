static int sockent_client_connect (sockent_t *se) /* {{{ */
{
	static c_complain_t complaint = C_COMPLAIN_INIT_STATIC;

	struct sockent_client *client;
	struct addrinfo  ai_hints;
	struct addrinfo *ai_list = NULL, *ai_ptr;
	int status;

	if ((se == NULL) || (se->type != SOCKENT_TYPE_CLIENT))
		return (EINVAL);

	client = &se->data.client;
	if (client->fd >= 0) /* already connected */
		return (0);

	memset (&ai_hints, 0, sizeof (ai_hints));
#ifdef AI_ADDRCONFIG
	ai_hints.ai_flags |= AI_ADDRCONFIG;
#endif
	ai_hints.ai_family   = AF_UNSPEC;
	ai_hints.ai_socktype = SOCK_DGRAM;
	ai_hints.ai_protocol = IPPROTO_UDP;

	status = getaddrinfo (se->node,
			(se->service != NULL) ? se->service : NET_DEFAULT_PORT,
			&ai_hints, &ai_list);
	if (status != 0)
	{
		c_complain (LOG_ERR, &complaint,
				"network plugin: getaddrinfo (%s, %s) failed: %s",
				(se->node == NULL) ? "(null)" : se->node,
				(se->service == NULL) ? "(null)" : se->service,
				gai_strerror (status));
		return (-1);
	}
	else
	{
		c_release (LOG_NOTICE, &complaint,
				"network plugin: Successfully resolved \"%s\".",
				se->node);
	}

	for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next)
	{
		client->fd = socket (ai_ptr->ai_family,
				ai_ptr->ai_socktype,
				ai_ptr->ai_protocol);
		if (client->fd < 0)
		{
			char errbuf[1024];
			ERROR ("network plugin: socket(2) failed: %s",
					sstrerror (errno, errbuf,
						sizeof (errbuf)));
			continue;
		}

		client->addr = malloc (sizeof (*client->addr));
		if (client->addr == NULL)
		{
			ERROR ("network plugin: malloc failed.");
			close (client->fd);
			client->fd = -1;
			continue;
		}

		memset (client->addr, 0, sizeof (*client->addr));
		assert (sizeof (*client->addr) >= ai_ptr->ai_addrlen);
		memcpy (client->addr, ai_ptr->ai_addr, ai_ptr->ai_addrlen);
		client->addrlen = ai_ptr->ai_addrlen;

		network_set_ttl (se, ai_ptr);
		network_set_interface (se, ai_ptr);

		/* We don't open more than one write-socket per
		 * node/service pair.. */
		break;
	}

	freeaddrinfo (ai_list);
	if (client->fd < 0)
		return (-1);
	return (0);
} /* }}} int sockent_client_connect */