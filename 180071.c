ves_icall_get_method_info (MonoMethod *method, MonoMethodInfo *info)
{
	MonoDomain *domain = mono_domain_get ();
	MonoMethodSignature* sig;
	MONO_ARCH_SAVE_REGS;

	sig = mono_method_signature (method);
	if (!sig) {
		g_assert (mono_loader_get_last_error ());
		mono_raise_exception (mono_loader_error_prepare_exception (mono_loader_get_last_error ()));
	}

	MONO_STRUCT_SETREF (info, parent, mono_type_get_object (domain, &method->klass->byval_arg));
	MONO_STRUCT_SETREF (info, ret, mono_type_get_object (domain, sig->ret));
	info->attrs = method->flags;
	info->implattrs = method->iflags;
	if (sig->call_convention == MONO_CALL_DEFAULT)
		info->callconv = sig->sentinelpos >= 0 ? 2 : 1;
	else {
		if (sig->call_convention == MONO_CALL_VARARG || sig->sentinelpos >= 0)
			info->callconv = 2;
		else
			info->callconv = 1;
	}
	info->callconv |= (sig->hasthis << 5) | (sig->explicit_this << 6); 
}