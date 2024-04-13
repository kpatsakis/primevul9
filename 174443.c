process_update_pdu(STREAM s)
{
	uint16 update_type, count;

	in_uint16_le(s, update_type);

	ui_begin_update();
	switch (update_type)
	{
		case RDP_UPDATE_ORDERS:
			in_uint8s(s, 2);	/* pad */
			in_uint16_le(s, count);
			in_uint8s(s, 2);	/* pad */
			process_orders(s, count);
			break;

		case RDP_UPDATE_BITMAP:
			process_bitmap_updates(s);
			break;

		case RDP_UPDATE_PALETTE:
			process_palette(s);
			break;

		case RDP_UPDATE_SYNCHRONIZE:
			break;

		default:
			unimpl("update %d\n", update_type);
	}
	ui_end_update();
}