ves_icall_System_Reflection_Assembly_get_location (MonoReflectionAssembly *assembly)
{
	MonoDomain *domain = mono_object_domain (assembly); 
	MonoString *res;

	MONO_ARCH_SAVE_REGS;

	res = mono_string_new (domain, mono_image_get_filename (assembly->assembly->image));

	return res;
}