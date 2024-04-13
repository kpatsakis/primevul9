ves_icall_System_Reflection_Module_GetGuidInternal (MonoReflectionModule *module)
{
	MonoDomain *domain = mono_object_domain (module); 

	MONO_ARCH_SAVE_REGS;

	g_assert (module->image);
	return mono_string_new (domain, module->image->guid);
}