soup_auth_ntlm_get_protection_space (SoupAuth *auth, SoupURI *source_uri)
{
	char *space, *p;

	space = g_strdup (source_uri->path);

	/* Strip filename component */
	p = strrchr (space, '/');
	if (p && p != space && p[1])
		*p = '\0';

	return g_slist_prepend (NULL, space);
}