quit_mainloop (NMVPNCPlugin *plugin, gpointer user_data)
{
	g_main_loop_quit ((GMainLoop *) user_data);
}