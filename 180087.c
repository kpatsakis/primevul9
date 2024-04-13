ves_icall_ModuleBuilder_getToken (MonoReflectionModuleBuilder *mb, MonoObject *obj)
{
	MONO_ARCH_SAVE_REGS;
	
	MONO_CHECK_ARG_NULL (obj);
	
	return mono_image_create_token (mb->dynamic_image, obj, TRUE, TRUE);
}