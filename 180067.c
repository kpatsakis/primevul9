ves_icall_System_Reflection_Assembly_get_global_assembly_cache (MonoReflectionAssembly *assembly)
{
	MonoAssembly *mass = assembly->assembly;

	MONO_ARCH_SAVE_REGS;

	return mass->in_gac;
}