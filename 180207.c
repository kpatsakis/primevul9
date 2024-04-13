ves_icall_System_Reflection_Assembly_FillName (MonoReflectionAssembly *assembly, MonoReflectionAssemblyName *aname)
{
	gchar *absolute;
	MonoAssembly *mass = assembly->assembly;

	MONO_ARCH_SAVE_REGS;

	if (g_path_is_absolute (mass->image->name)) {
		fill_reflection_assembly_name (mono_object_domain (assembly),
			aname, &mass->aname, mass->image->name, TRUE,
			TRUE, mono_framework_version () >= 2);
		return;
	}
	absolute = g_build_filename (mass->basedir, mass->image->name, NULL);

	fill_reflection_assembly_name (mono_object_domain (assembly),
		aname, &mass->aname, absolute, TRUE, TRUE,
		mono_framework_version () >= 2);

	g_free (absolute);
}