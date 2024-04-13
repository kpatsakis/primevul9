rdp_main_loop(RD_BOOL * deactivated, uint32 * ext_disc_reason)
{
	while (rdp_loop(deactivated, ext_disc_reason))
	{
		if (g_pending_resize || g_redirect)
		{
			return;
		}
	}
}