sec_send(STREAM s, uint32 flags)
{
	sec_send_to_channel(s, flags, MCS_GLOBAL_CHANNEL);
}