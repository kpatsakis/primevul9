ves_icall_System_Reflection_Assembly_load_with_partial_name (MonoString *mname, MonoObject *evidence)
{
	gchar *name;
	MonoAssembly *res;
	MonoImageOpenStatus status;
	
	MONO_ARCH_SAVE_REGS;

	name = mono_string_to_utf8 (mname);
	res = mono_assembly_load_with_partial_name (name, &status);

	g_free (name);

	if (res == NULL)
		return NULL;
	return mono_assembly_get_object (mono_domain_get (), res);
}