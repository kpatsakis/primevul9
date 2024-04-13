ves_icall_System_Reflection_Module_InternalGetTypes (MonoReflectionModule *module)
{
	MonoArray *exceptions;
	int i;

	MONO_ARCH_SAVE_REGS;

	if (!module->image)
		return mono_array_new (mono_object_domain (module), mono_defaults.monotype_class, 0);
	else {
		MonoArray *res = mono_module_get_types (mono_object_domain (module), module->image, &exceptions, FALSE);
		for (i = 0; i < mono_array_length (exceptions); ++i) {
			MonoException *ex = mono_array_get (exceptions, MonoException *, i);
			if (ex)
				mono_raise_exception (ex);
		}
		return res;
	}
}