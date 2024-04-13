ves_icall_Mono_Runtime_GetDisplayName (void)
{
	char *info;
	MonoString *display_name;

	info = mono_get_runtime_callbacks ()->get_runtime_build_info ();
	display_name = mono_string_new (mono_domain_get (), info);
	g_free (info);
	return display_name;
}