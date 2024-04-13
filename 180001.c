ves_icall_System_Reflection_Assembly_get_ReflectionOnly (MonoReflectionAssembly *assembly)
{
	MONO_ARCH_SAVE_REGS;

	return assembly->assembly->ref_only;
}