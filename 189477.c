e_source_registry_debug_enabled (void)
{
	static gint esr_debug = -1;

	if (esr_debug == -1)
		esr_debug = g_strcmp0 (g_getenv ("ESR_DEBUG"), "1") == 0 ? 1 : 0;

	return esr_debug == 1;
}