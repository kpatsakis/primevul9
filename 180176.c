ves_icall_System_Delegate_CreateDelegate_internal (MonoReflectionType *type, MonoObject *target,
						   MonoReflectionMethod *info, MonoBoolean throwOnBindFailure)
{
	MonoClass *delegate_class = mono_class_from_mono_type (type->type);
	MonoObject *delegate;
	gpointer func;
	MonoMethod *method = info->method;

	MONO_ARCH_SAVE_REGS;

	mono_assert (delegate_class->parent == mono_defaults.multicastdelegate_class);

	if (mono_security_get_mode () == MONO_SECURITY_MODE_CORE_CLR) {
		if (!mono_security_core_clr_ensure_delegate_creation (method, throwOnBindFailure))
			return NULL;
	}

	delegate = mono_object_new (mono_object_domain (type), delegate_class);

	if (method->dynamic) {
		/* Creating a trampoline would leak memory */
		func = mono_compile_method (method);
	} else {
		func = mono_create_ftnptr (mono_domain_get (),
			mono_runtime_create_jump_trampoline (mono_domain_get (), method, TRUE));
	}

	mono_delegate_ctor_with_method (delegate, target, func, method);

	return delegate;
}