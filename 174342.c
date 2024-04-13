process_pointer_pdu(STREAM s)
{
	uint16 message_type;
	uint16 x, y;

	in_uint16_le(s, message_type);
	in_uint8s(s, 2);	/* pad */

	switch (message_type)
	{
		case RDP_POINTER_MOVE:
			in_uint16_le(s, x);
			in_uint16_le(s, y);
			if (s_check(s))
				ui_move_pointer(x, y);
			break;

		case RDP_POINTER_COLOR:
			process_colour_pointer_pdu(s);
			break;

		case RDP_POINTER_CACHED:
			process_cached_pointer_pdu(s);
			break;

		case RDP_POINTER_SYSTEM:
			process_system_pointer_pdu(s);
			break;

		case RDP_POINTER_NEW:
			process_new_pointer_pdu(s);
			break;

		default:
			unimpl("Pointer message 0x%x\n", message_type);
	}
}