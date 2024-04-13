mcs_send(STREAM s)
{
	mcs_send_to_channel(s, MCS_GLOBAL_CHANNEL);
}