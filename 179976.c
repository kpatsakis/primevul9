ves_icall_MonoType_GetGenericArguments (MonoReflectionType *type)
{
	MonoArray *res;
	MonoClass *klass, *pklass;
	MonoDomain *domain = mono_object_domain (type);
	MonoVTable *array_vtable = mono_class_vtable_full (domain, mono_array_class_get_cached (mono_defaults.systemtype_class, 1), TRUE);
	int i;
	MONO_ARCH_SAVE_REGS;

	klass = mono_class_from_mono_type (type->type);

	if (klass->generic_container) {
		MonoGenericContainer *container = klass->generic_container;
		res = mono_array_new_specific (array_vtable, container->type_argc);
		for (i = 0; i < container->type_argc; ++i) {
			pklass = mono_class_from_generic_parameter (mono_generic_container_get_param (container, i), klass->image, FALSE);
			mono_array_setref (res, i, mono_type_get_object (domain, &pklass->byval_arg));
		}
	} else if (klass->generic_class) {
		MonoGenericInst *inst = klass->generic_class->context.class_inst;
		res = mono_array_new_specific (array_vtable, inst->type_argc);
		for (i = 0; i < inst->type_argc; ++i)
			mono_array_setref (res, i, mono_type_get_object (domain, inst->type_argv [i]));
	} else {
		res = mono_array_new_specific (array_vtable, 0);
	}
	return res;
}