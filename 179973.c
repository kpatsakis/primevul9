ves_icall_ModuleBuilder_RegisterToken (MonoReflectionModuleBuilder *mb, MonoObject *obj, guint32 token)
{
	MONO_ARCH_SAVE_REGS;

	mono_image_register_token (mb->dynamic_image, token, obj);
}