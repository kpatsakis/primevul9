static sockent_t *sockent_create (int type) /* {{{ */
{
	sockent_t *se;

	if ((type != SOCKENT_TYPE_CLIENT) && (type != SOCKENT_TYPE_SERVER))
		return (NULL);

	se = malloc (sizeof (*se));
	if (se == NULL)
		return (NULL);
	memset (se, 0, sizeof (*se));

	se->type = type;
	se->node = NULL;
	se->service = NULL;
	se->interface = 0;
	se->next = NULL;

	if (type == SOCKENT_TYPE_SERVER)
	{
		se->data.server.fd = NULL;
		se->data.server.fd_num = 0;
#if HAVE_LIBGCRYPT
		se->data.server.security_level = SECURITY_LEVEL_NONE;
		se->data.server.auth_file = NULL;
		se->data.server.userdb = NULL;
		se->data.server.cypher = NULL;
#endif
	}
	else
	{
		se->data.client.fd = -1;
		se->data.client.addr = NULL;
#if HAVE_LIBGCRYPT
		se->data.client.security_level = SECURITY_LEVEL_NONE;
		se->data.client.username = NULL;
		se->data.client.password = NULL;
		se->data.client.cypher = NULL;
#endif
	}

	return (se);
} /* }}} sockent_t *sockent_create */