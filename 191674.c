static int sockent_init_crypto (sockent_t *se) /* {{{ */
{
#if HAVE_LIBGCRYPT /* {{{ */
	if (se->type == SOCKENT_TYPE_CLIENT)
	{
		if (se->data.client.security_level > SECURITY_LEVEL_NONE)
		{
			network_init_gcrypt ();

			if ((se->data.client.username == NULL)
					|| (se->data.client.password == NULL))
			{
				ERROR ("network plugin: Client socket with "
						"security requested, but no "
						"credentials are configured.");
				return (-1);
			}
			gcry_md_hash_buffer (GCRY_MD_SHA256,
					se->data.client.password_hash,
					se->data.client.password,
					strlen (se->data.client.password));
		}
	}
	else /* (se->type == SOCKENT_TYPE_SERVER) */
	{
		if (se->data.server.security_level > SECURITY_LEVEL_NONE)
		{
			network_init_gcrypt ();

			if (se->data.server.auth_file == NULL)
			{
				ERROR ("network plugin: Server socket with "
						"security requested, but no "
						"password file is configured.");
				return (-1);
			}
		}
		if (se->data.server.auth_file != NULL)
		{
			se->data.server.userdb = fbh_create (se->data.server.auth_file);
			if (se->data.server.userdb == NULL)
			{
				ERROR ("network plugin: Reading password file "
						"`%s' failed.",
						se->data.server.auth_file);
				if (se->data.server.security_level > SECURITY_LEVEL_NONE)
					return (-1);
			}
		}
	}
#endif /* }}} HAVE_LIBGCRYPT */

	return (0);
} /* }}} int sockent_init_crypto */