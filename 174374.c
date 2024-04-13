rdp_reset_state(void)
{
	g_next_packet = NULL;	/* reset the packet information */
	g_rdp_shareid = 0;
	sec_reset_state();
}