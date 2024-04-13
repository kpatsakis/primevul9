process_pdu_logon(STREAM s)
{
	uint32 infotype;
	in_uint32_le(s, infotype);
	if (infotype == INFOTYPE_LOGON_EXTENDED_INF)
	{
		uint32 fieldspresent;

		in_uint8s(s, 2);	/* Length */
		in_uint32_le(s, fieldspresent);
		if (fieldspresent & LOGON_EX_AUTORECONNECTCOOKIE)
		{
			uint32 len;
			uint32 version;

			/* TS_LOGON_INFO_FIELD */
			in_uint8s(s, 4);	/* cbFieldData */

			/* ARC_SC_PRIVATE_PACKET */
			in_uint32_le(s, len);
			if (len != 28)
			{
				warning("Invalid length in Auto-Reconnect packet\n");
				return;
			}

			in_uint32_le(s, version);
			if (version != 1)
			{
				warning("Unsupported version of Auto-Reconnect packet\n");
				return;
			}

			in_uint32_le(s, g_reconnect_logonid);
			in_uint8a(s, g_reconnect_random, 16);
			g_has_reconnect_random = True;
			g_reconnect_random_ts = time(NULL);
			DEBUG(("Saving auto-reconnect cookie, id=%u\n", g_reconnect_logonid));
		}
	}
}