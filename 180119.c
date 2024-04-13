ves_icall_System_Reflection_AssemblyName_ParseName (MonoReflectionAssemblyName *name, MonoString *assname)
{
	MonoAssemblyName aname;
	MonoDomain *domain = mono_object_domain (name);
	char *val;
	gboolean is_version_defined;
	gboolean is_token_defined;

	aname.public_key = NULL;
	val = mono_string_to_utf8 (assname);
	if (!mono_assembly_name_parse_full (val, &aname, TRUE, &is_version_defined, &is_token_defined)) {
		g_free ((guint8*) aname.public_key);
		g_free (val);
		return FALSE;
	}
	
	fill_reflection_assembly_name (domain, name, &aname, "", is_version_defined,
		FALSE, is_token_defined);

	mono_assembly_name_free (&aname);
	g_free ((guint8*) aname.public_key);
	g_free (val);

	return TRUE;
}