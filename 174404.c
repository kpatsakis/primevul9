process_disconnect_pdu(STREAM s, uint32 * ext_disc_reason)
{
	in_uint32_le(s, *ext_disc_reason);

	DEBUG(("Received disconnect PDU\n"));
}