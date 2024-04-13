ves_icall_System_Environment_GetGacPath (void)
{
	return mono_string_new (mono_domain_get (), mono_assembly_getrootdir ());
}