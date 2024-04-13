ves_icall_MonoMethod_get_base_definition (MonoReflectionMethod *m)
{
	MonoClass *klass, *parent;
	MonoMethod *method = m->method;
	MonoMethod *result = NULL;

	MONO_ARCH_SAVE_REGS;

	if (method->klass == NULL)
		return m;

	if (!(method->flags & METHOD_ATTRIBUTE_VIRTUAL) ||
	    MONO_CLASS_IS_INTERFACE (method->klass) ||
	    method->flags & METHOD_ATTRIBUTE_NEW_SLOT)
		return m;

	klass = method->klass;
	if (klass->generic_class)
		klass = klass->generic_class->container_class;

	/* At the end of the loop, klass points to the eldest class that has this virtual function slot. */
	for (parent = klass->parent; parent != NULL; parent = parent->parent) {
		mono_class_setup_vtable (parent);
		if (parent->vtable_size <= method->slot)
			break;
		klass = parent;
	}		

	if (klass == method->klass)
		return m;

	result = klass->vtable [method->slot];
	if (result == NULL) {
		/* It is an abstract method */
		gpointer iter = NULL;
		while ((result = mono_class_get_methods (klass, &iter)))
			if (result->slot == method->slot)
				break;
	}

	if (result == NULL)
		return m;

	return mono_method_get_object (mono_domain_get (), result, NULL);
}