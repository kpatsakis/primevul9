rdp_send_client_window_status(int status)
{
	STREAM s;
	static int current_status = 1;

	if (current_status == status)
		return;

	s = rdp_init_data(12);

	out_uint32_le(s, status);

	switch (status)
	{
		case 0:	/* shut the server up */
			break;

		case 1:	/* receive data again */
			out_uint32_le(s, 0);	/* unknown */
			out_uint16_le(s, g_width);
			out_uint16_le(s, g_height);
			break;
	}

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_CLIENT_WINDOW_STATUS);
	current_status = status;
}