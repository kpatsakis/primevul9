seamless_send_position(unsigned long id, int x, int y, int width, int height, unsigned long flags)
{
	return seamless_send("POSITION", "0x%08lx,%d,%d,%d,%d,0x%lx", id, x, y, width, height,
			     flags);
}