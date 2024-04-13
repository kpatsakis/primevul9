ves_icall_System_Reflection_Assembly_GetEntryAssembly (void)
{
	MonoDomain* domain = mono_domain_get ();

	MONO_ARCH_SAVE_REGS;

	if (!domain->entry_assembly)
		return NULL;

	return mono_assembly_get_object (domain, domain->entry_assembly);
}