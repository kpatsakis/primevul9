seamless_send_focus(unsigned long id, unsigned long flags)
{
	if (!g_seamless_rdp)
		return (unsigned int) -1;

	return seamless_send("FOCUS", "0x%08lx,0x%lx", id, flags);
}