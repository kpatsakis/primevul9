ves_icall_System_Reflection_Assembly_InternalImageRuntimeVersion (MonoReflectionAssembly *assembly)
{
	MonoDomain *domain = mono_object_domain (assembly); 

	MONO_ARCH_SAVE_REGS;

	return mono_string_new (domain, assembly->assembly->image->version);
}