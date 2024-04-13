process_redirect_pdu(STREAM s, RD_BOOL enhanced_redirect /*, uint32 * ext_disc_reason */ )
{
	uint32 len;
	uint16 redirect_identifier;

	/* reset any previous redirection information */
	g_redirect = True;
	free(g_redirect_server);
	free(g_redirect_username);
	free(g_redirect_domain);
	free(g_redirect_lb_info);
	free(g_redirect_cookie);

	g_redirect_server = NULL;
	g_redirect_username = NULL;
	g_redirect_domain = NULL;
	g_redirect_lb_info = NULL;
	g_redirect_cookie = NULL;

	/* these 2 bytes are unknown, seem to be zeros */
	in_uint8s(s, 2);

	/* FIXME: Previous implementation only reads 4 bytes which has been working
	   but todays spec says something different. Investigate and retest
	   server redirection using WTS 2003 cluster.
	 */

	if (enhanced_redirect)
	{
		/* read identifier */
		in_uint16_le(s, redirect_identifier);
		if (redirect_identifier != 0x0400)
			error("Protocol error in server redirection, unexpected data.");

		/* FIXME: skip total length */
		in_uint8s(s, 2);

		/* read session_id */
		in_uint32_le(s, g_redirect_session_id);
	}

	/* read connection flags */
	in_uint32_le(s, g_redirect_flags);

	if (g_redirect_flags & PDU_REDIRECT_HAS_IP)
	{
		/* read length of ip string */
		in_uint32_le(s, len);

		/* read ip string */
		rdp_in_unistr(s, len, &g_redirect_server, &g_redirect_server_len);
	}

	if (g_redirect_flags & PDU_REDIRECT_HAS_LOAD_BALANCE_INFO)
	{
		/* read length of load balance info blob */
		in_uint32_le(s, g_redirect_lb_info_len);

		/* reallocate a loadbalance info blob */
		if (g_redirect_lb_info != NULL)
			free(g_redirect_lb_info);

		g_redirect_lb_info = xmalloc(g_redirect_lb_info_len);

		/* read load balance info blob */
		in_uint8p(s, g_redirect_lb_info, g_redirect_lb_info_len);
	}

	if (g_redirect_flags & PDU_REDIRECT_HAS_USERNAME)
	{
		/* read length of username string */
		in_uint32_le(s, len);

		/* read username string */
		rdp_in_unistr(s, len, &g_redirect_username, &g_redirect_username_len);
	}

	if (g_redirect_flags & PDU_REDIRECT_HAS_DOMAIN)
	{
		/* read length of domain string */
		in_uint32_le(s, len);

		/* read domain string */
		rdp_in_unistr(s, len, &g_redirect_domain, &g_redirect_domain_len);
	}

	if (g_redirect_flags & PDU_REDIRECT_HAS_PASSWORD)
	{
		/* the information in this blob is either a password or a cookie that
		   should be passed though as blob and not parsed as a unicode string */

		/* read blob length */
		in_uint32_le(s, g_redirect_cookie_len);

		/* reallocate cookie blob */
		if (g_redirect_cookie != NULL)
			free(g_redirect_cookie);

		g_redirect_cookie = xmalloc(g_redirect_cookie_len);

		/* read cookie as is */
		in_uint8p(s, g_redirect_cookie, g_redirect_cookie_len);
	}

	if (g_redirect_flags & PDU_REDIRECT_DONT_STORE_USERNAME)
	{
		warning("PDU_REDIRECT_DONT_STORE_USERNAME set\n");
	}

	if (g_redirect_flags & PDU_REDIRECT_USE_SMARTCARD)
	{
		warning("PDU_REDIRECT_USE_SMARTCARD set\n");
	}

	if (g_redirect_flags & PDU_REDIRECT_INFORMATIONAL)
	{
		/* By spec this is only for information and doesn't mean that an actual
		   redirect should be performed. How it should be used is not mentioned. */
		g_redirect = False;
	}

	if (g_redirect_flags & PDU_REDIRECT_HAS_TARGET_FQDN)
	{
		in_uint32_le(s, len);

		/* Let target fqdn replace target ip address */
		if (g_redirect_server)
		{
			free(g_redirect_server);
			g_redirect_server = NULL;
		}

		/* read fqdn string */
		rdp_in_unistr(s, len, &g_redirect_server, &g_redirect_server_len);
	}

	if (g_redirect_flags & PDU_REDIRECT_HAS_TARGET_NETBIOS)
	{
		warning("PDU_REDIRECT_HAS_TARGET_NETBIOS set\n");
	}

	if (g_redirect_flags & PDU_REDIRECT_HAS_TARGET_IP_ARRAY)
	{
		warning("PDU_REDIRECT_HAS_TARGET_IP_ARRAY set\n");
	}

	return True;
}