seamless_send_zchange(unsigned long id, unsigned long below, unsigned long flags)
{
	if (!g_seamless_rdp)
		return (unsigned int) -1;

	return seamless_send("ZCHANGE", "0x%08lx,0x%08lx,0x%lx", id, below, flags);
}