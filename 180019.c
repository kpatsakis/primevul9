ves_icall_Remoting_RemotingServices_GetVirtualMethod (
	MonoReflectionType *rtype, MonoReflectionMethod *rmethod)
{
	MonoClass *klass;
	MonoMethod *method;
	MonoMethod **vtable;
	MonoMethod *res = NULL;

	MONO_CHECK_ARG_NULL (rtype);
	MONO_CHECK_ARG_NULL (rmethod);

	method = rmethod->method;
	klass = mono_class_from_mono_type (rtype->type);

	if (MONO_CLASS_IS_INTERFACE (klass))
		return NULL;

	if (method->flags & METHOD_ATTRIBUTE_STATIC)
		return NULL;

	if ((method->flags & METHOD_ATTRIBUTE_FINAL) || !(method->flags & METHOD_ATTRIBUTE_VIRTUAL)) {
		if (klass == method->klass || mono_class_is_subclass_of (klass, method->klass, FALSE))
			return rmethod;
		else
			return NULL;
	}

	mono_class_setup_vtable (klass);
	vtable = klass->vtable;

	if (method->klass->flags & TYPE_ATTRIBUTE_INTERFACE) {
		int offs = mono_class_interface_offset (klass, method->klass);
		if (offs >= 0)
			res = vtable [offs + method->slot];
	} else {
		if (!(klass == method->klass || mono_class_is_subclass_of (klass, method->klass, FALSE)))
			return NULL;

		if (method->slot != -1)
			res = vtable [method->slot];
	}

	if (!res)
		return NULL;

	return mono_method_get_object (mono_domain_get (), res, NULL);
}