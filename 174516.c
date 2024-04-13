seamless_send_state(unsigned long id, unsigned int state, unsigned long flags)
{
	if (!g_seamless_rdp)
		return (unsigned int) -1;

	return seamless_send("STATE", "0x%08lx,0x%x,0x%lx", id, state, flags);
}