ves_icall_System_Runtime_Activation_ActivationServices_EnableProxyActivation (MonoReflectionType *type, MonoBoolean enable)
{
	MonoClass *klass;
	MonoVTable* vtable;

	MONO_ARCH_SAVE_REGS;

	klass = mono_class_from_mono_type (type->type);
	vtable = mono_class_vtable_full (mono_domain_get (), klass, TRUE);

	if (enable) vtable->remote = 1;
	else vtable->remote = 0;
}