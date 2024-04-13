ves_icall_System_Runtime_Activation_ActivationServices_AllocateUninitializedClassInstance (MonoReflectionType *type)
{
	MonoClass *klass;
	MonoDomain *domain;
	
	MONO_ARCH_SAVE_REGS;

	domain = mono_object_domain (type);
	klass = mono_class_from_mono_type (type->type);

	if (klass->rank >= 1) {
		g_assert (klass->rank == 1);
		return (MonoObject *) mono_array_new (domain, klass->element_class, 0);
	} else {
		/* Bypass remoting object creation check */
		return mono_object_new_alloc_specific (mono_class_vtable_full (domain, klass, TRUE));
	}
}