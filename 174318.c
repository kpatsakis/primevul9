rdp_send_logon_info(uint32 flags, char *domain, char *user,
		    char *password, char *program, char *directory)
{
	char *ipaddr = tcp_get_address();
	/* length of string in TS_INFO_PACKET excludes null terminator */
	int len_domain = 2 * strlen(domain);
	int len_user = 2 * strlen(user);
	int len_password = 2 * strlen(password);
	int len_program = 2 * strlen(program);
	int len_directory = 2 * strlen(directory);

	/* length of strings in TS_EXTENDED_PACKET includes null terminator */
	int len_ip = 2 * strlen(ipaddr) + 2;
	int len_dll = 2 * strlen("C:\\WINNT\\System32\\mstscax.dll") + 2;

	int packetlen = 0;
	uint32 sec_flags = g_encryption ? (SEC_LOGON_INFO | SEC_ENCRYPT) : SEC_LOGON_INFO;
	STREAM s;
	time_t t = time(NULL);
	time_t tzone;
	uint8 security_verifier[16];

	if (g_rdp_version == RDP_V4 || 1 == g_server_rdp_version)
	{
		DEBUG_RDP5(("Sending RDP4-style Logon packet\n"));

		s = sec_init(sec_flags, 18 + len_domain + len_user + len_password
			     + len_program + len_directory + 10);

		out_uint32(s, 0);
		out_uint32_le(s, flags);
		out_uint16_le(s, len_domain);
		out_uint16_le(s, len_user);
		out_uint16_le(s, len_password);
		out_uint16_le(s, len_program);
		out_uint16_le(s, len_directory);
		rdp_out_unistr(s, domain, len_domain);
		rdp_out_unistr(s, user, len_user);
		rdp_out_unistr(s, password, len_password);
		rdp_out_unistr(s, program, len_program);
		rdp_out_unistr(s, directory, len_directory);
	}
	else
	{

		DEBUG_RDP5(("Sending RDP5-style Logon packet\n"));

		if (g_redirect == True && g_redirect_cookie_len > 0)
		{
			len_password = g_redirect_cookie_len;
			len_password -= 2;	/* substract 2 bytes which is added below */
		}

		packetlen =
			/* size of TS_INFO_PACKET */
			4 +	/* CodePage */
			4 +	/* flags */
			2 +	/* cbDomain */
			2 +	/* cbUserName */
			2 +	/* cbPassword */
			2 +	/* cbAlternateShell */
			2 +	/* cbWorkingDir */
			2 + len_domain +	/* Domain */
			2 + len_user +	/* UserName */
			2 + len_password +	/* Password */
			2 + len_program +	/* AlternateShell */
			2 + len_directory +	/* WorkingDir */
			/* size of TS_EXTENDED_INFO_PACKET */
			2 +	/* clientAdressFamily */
			2 +	/* cbClientAdress */
			len_ip +	/* clientAddress */
			2 +	/* cbClientDir */
			len_dll +	/* clientDir */
			/* size of TS_TIME_ZONE_INFORMATION */
			4 +	/* Bias, (UTC = local time + bias */
			64 +	/* StandardName, 32 unicode char array, Descriptive standard time on client */
			16 +	/* StandardDate */
			4 +	/* StandardBias */
			64 +	/* DaylightName, 32 unicode char array */
			16 +	/* DaylightDate */
			4 +	/* DaylightBias */
			4 +	/* clientSessionId */
			4 +	/* performanceFlags */
			2 +	/* cbAutoReconnectCookie, either 0 or 0x001c */
			/* size of ARC_CS_PRIVATE_PACKET */
			28;	/* autoReconnectCookie */


		s = sec_init(sec_flags, packetlen);
		DEBUG_RDP5(("Called sec_init with packetlen %d\n", packetlen));

		/* TS_INFO_PACKET */
		out_uint32(s, 0);	/* Code Page */
		out_uint32_le(s, flags);
		out_uint16_le(s, len_domain);
		out_uint16_le(s, len_user);
		out_uint16_le(s, len_password);
		out_uint16_le(s, len_program);
		out_uint16_le(s, len_directory);

		if (0 < len_domain)
			rdp_out_unistr(s, domain, len_domain);
		else
			out_uint16_le(s, 0);	/* mandatory 2 bytes null terminator */

		if (0 < len_user)
			rdp_out_unistr(s, user, len_user);
		else
			out_uint16_le(s, 0);	/* mandatory 2 bytes null terminator */

		if (0 < len_password)
		{
			if (g_redirect == True && 0 < g_redirect_cookie_len)
			{
				out_uint8p(s, g_redirect_cookie, g_redirect_cookie_len);
			}
			else
			{
				rdp_out_unistr(s, password, len_password);
			}
		}
		else
			out_uint16_le(s, 0);	/* mandatory 2 bytes null terminator */

		if (0 < len_program)
			rdp_out_unistr(s, program, len_program);
		else
			out_uint16_le(s, 0);	/* mandatory 2 bytes null terminator */

		if (0 < len_directory)
			rdp_out_unistr(s, directory, len_directory);
		else
			out_uint16_le(s, 0);	/* mandatory 2 bytes null terminator */

		/* TS_EXTENDED_INFO_PACKET */
		out_uint16_le(s, 2);	/* clientAddressFamily = AF_INET */
		out_uint16_le(s, len_ip);	/* cbClientAddress, Length of client ip */
		rdp_out_unistr(s, ipaddr, len_ip - 2);	/* clientAddress */
		out_uint16_le(s, len_dll);	/* cbClientDir */
		rdp_out_unistr(s, "C:\\WINNT\\System32\\mstscax.dll", len_dll - 2);	/* clientDir */

		/* TS_TIME_ZONE_INFORMATION */
		tzone = (mktime(gmtime(&t)) - mktime(localtime(&t))) / 60;
		out_uint32_le(s, tzone);
		rdp_out_unistr(s, "GTB, normaltid", 2 * strlen("GTB, normaltid"));
		out_uint8s(s, 62 - 2 * strlen("GTB, normaltid"));
		out_uint32_le(s, 0x0a0000);
		out_uint32_le(s, 0x050000);
		out_uint32_le(s, 3);
		out_uint32_le(s, 0);
		out_uint32_le(s, 0);
		rdp_out_unistr(s, "GTB, sommartid", 2 * strlen("GTB, sommartid"));
		out_uint8s(s, 62 - 2 * strlen("GTB, sommartid"));
		out_uint32_le(s, 0x30000);
		out_uint32_le(s, 0x050000);
		out_uint32_le(s, 2);
		out_uint32(s, 0);
		out_uint32_le(s, 0xffffffc4);	/* DaylightBias */

		/* Rest of TS_EXTENDED_INFO_PACKET */
		out_uint32_le(s, 0);	/* clientSessionId (Ignored by server MUST be 0) */
		out_uint32_le(s, g_rdp5_performanceflags);

		/* Client Auto-Reconnect */
		if (g_has_reconnect_random)
		{
			out_uint16_le(s, 28);	/* cbAutoReconnectLen */
			/* ARC_CS_PRIVATE_PACKET */
			out_uint32_le(s, 28);	/* cbLen */
			out_uint32_le(s, 1);	/* Version */
			out_uint32_le(s, g_reconnect_logonid);	/* LogonId */
			rdssl_hmac_md5(g_reconnect_random, sizeof(g_reconnect_random),
				       g_client_random, SEC_RANDOM_SIZE, security_verifier);
			out_uint8a(s, security_verifier, sizeof(security_verifier));
		}
		else
		{
			out_uint16_le(s, 0);	/* cbAutoReconnectLen */
		}

	}
	s_mark_end(s);

	/* clear the redirect flag */
	g_redirect = False;

	sec_send(s, sec_flags);
}