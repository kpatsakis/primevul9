ves_icall_ModuleBuilder_getMethodToken (MonoReflectionModuleBuilder *mb,
					MonoReflectionMethod *method,
					MonoArray *opt_param_types)
{
	MONO_ARCH_SAVE_REGS;

	MONO_CHECK_ARG_NULL (method);
	
	return mono_image_create_method_token (
		mb->dynamic_image, (MonoObject *) method, opt_param_types);
}