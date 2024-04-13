rdp_process_bitmap_caps(STREAM s)
{
	uint16 width, height, depth;

	in_uint16_le(s, depth);
	in_uint8s(s, 6);

	in_uint16_le(s, width);
	in_uint16_le(s, height);

	DEBUG(("setting desktop size and depth to: %dx%dx%d\n", width, height, depth));

	/*
	 * The server may limit depth and change the size of the desktop (for
	 * example when shadowing another session).
	 */
	if (g_server_depth != depth)
	{
		warning("Remote desktop does not support colour depth %d; falling back to %d\n",
			g_server_depth, depth);
		g_server_depth = depth;
	}
	if (g_width != width || g_height != height)
	{
		warning("Remote desktop changed from %dx%d to %dx%d.\n", g_width, g_height,
			width, height);
		g_width = width;
		g_height = height;
		ui_resize_window();
	}
}