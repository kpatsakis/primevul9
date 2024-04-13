rdpdr_send_client_device_list_announce(void)
{
	/* DR_CORE_CLIENT_ANNOUNCE_RSP */
	uint32 driverlen, printerlen, bloblen;
	int i;
	STREAM s;
	PRINTER *printerinfo;

	s = channel_init(rdpdr_channel, announcedata_size());
	out_uint16_le(s, RDPDR_CTYP_CORE);
	out_uint16_le(s, PAKID_CORE_DEVICE_LIST_ANNOUNCE);

	out_uint32_le(s, g_num_devices);

	for (i = 0; i < g_num_devices; i++)
	{
		out_uint32_le(s, g_rdpdr_device[i].device_type);
		out_uint32_le(s, i);	/* RDP Device ID */
		/* Is it possible to use share names longer than 8 chars?
		   /astrand */
		out_uint8p(s, g_rdpdr_device[i].name, 8);

		switch (g_rdpdr_device[i].device_type)
		{
			case DEVICE_TYPE_PRINTER:
				printerinfo = (PRINTER *) g_rdpdr_device[i].pdevice_data;

				driverlen = 2 * strlen(printerinfo->driver) + 2;
				printerlen = 2 * strlen(printerinfo->printer) + 2;
				bloblen = printerinfo->bloblen;

				out_uint32_le(s, 24 + driverlen + printerlen + bloblen);	/* length of extra info */
				out_uint32_le(s, printerinfo->default_printer ? 2 : 0);
				out_uint8s(s, 8);	/* unknown */
				out_uint32_le(s, driverlen);
				out_uint32_le(s, printerlen);
				out_uint32_le(s, bloblen);
				rdp_out_unistr(s, printerinfo->driver, driverlen - 2);
				rdp_out_unistr(s, printerinfo->printer, printerlen - 2);
				out_uint8a(s, printerinfo->blob, bloblen);

				if (printerinfo->blob)
					xfree(printerinfo->blob);	/* Blob is sent twice if reconnecting */
				break;
			default:
				out_uint32(s, 0);
		}
	}

	s_mark_end(s);
	channel_send(s, rdpdr_channel);
}