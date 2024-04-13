seamless_send_destroy(unsigned long id)
{
	return seamless_send("DESTROY", "0x%08lx", id);
}