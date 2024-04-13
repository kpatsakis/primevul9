process_system_pointer_pdu(STREAM s)
{
	uint16 system_pointer_type;

	in_uint16_le(s, system_pointer_type);
	switch (system_pointer_type)
	{
		case RDP_NULL_POINTER:
			ui_set_null_cursor();
			break;

		default:
			unimpl("System pointer message 0x%x\n", system_pointer_type);
	}
}