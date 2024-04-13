ves_icall_System_Reflection_Assembly_GetCallingAssembly (void)
{
	MonoMethod *m;
	MonoMethod *dest;

	MONO_ARCH_SAVE_REGS;

	dest = NULL;
	mono_stack_walk_no_il (get_executing, &dest);
	m = dest;
	mono_stack_walk_no_il (get_caller, &dest);
	if (!dest)
		dest = m;
	return mono_assembly_get_object (mono_domain_get (), dest->klass->image->assembly);
}