ves_icall_System_Reflection_Assembly_get_fullName (MonoReflectionAssembly *assembly)
{
	MonoDomain *domain = mono_object_domain (assembly); 
	MonoAssembly *mass = assembly->assembly;
	MonoString *res;
	gchar *name;

	name = g_strdup_printf (
		"%s, Version=%d.%d.%d.%d, Culture=%s, PublicKeyToken=%s%s",
		mass->aname.name,
		mass->aname.major, mass->aname.minor, mass->aname.build, mass->aname.revision,
		mass->aname.culture && *mass->aname.culture? mass->aname.culture: "neutral",
		mass->aname.public_key_token [0] ? (char *)mass->aname.public_key_token : "null",
		(mass->aname.flags & ASSEMBLYREF_RETARGETABLE_FLAG) ? ", Retargetable=Yes" : "");

	res = mono_string_new (domain, name);
	g_free (name);

	return res;
}