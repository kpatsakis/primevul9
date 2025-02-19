ves_icall_System_Reflection_Module_GetGlobalType (MonoReflectionModule *module)
{
	MonoDomain *domain = mono_object_domain (module); 
	MonoClass *klass;

	MONO_ARCH_SAVE_REGS;

	g_assert (module->image);

	if (module->image->dynamic && ((MonoDynamicImage*)(module->image))->initial_image)
		/* These images do not have a global type */
		return NULL;

	klass = mono_class_get (module->image, 1 | MONO_TOKEN_TYPE_DEF);
	return mono_type_get_object (domain, &klass->byval_arg);
}