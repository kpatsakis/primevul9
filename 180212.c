ves_icall_Type_GetInterfaceMapData (MonoReflectionType *type, MonoReflectionType *iface, MonoArray **targets, MonoArray **methods)
{
	MonoClass *class = mono_class_from_mono_type (type->type);
	MonoClass *iclass = mono_class_from_mono_type (iface->type);
	MonoReflectionMethod *member;
	MonoMethod* method;
	gpointer iter;
	int i = 0, len, ioffset;
	MonoDomain *domain;

	MONO_ARCH_SAVE_REGS;

	mono_class_setup_vtable (class);

	/* type doesn't implement iface: the exception is thrown in managed code */
	if (! MONO_CLASS_IMPLEMENTS_INTERFACE (class, iclass->interface_id))
			return;

	len = mono_class_num_methods (iclass);
	ioffset = mono_class_interface_offset (class, iclass);
	domain = mono_object_domain (type);
	mono_gc_wbarrier_generic_store (targets, (MonoObject*) mono_array_new (domain, mono_defaults.method_info_class, len));
	mono_gc_wbarrier_generic_store (methods, (MonoObject*) mono_array_new (domain, mono_defaults.method_info_class, len));
	iter = NULL;
	while ((method = mono_class_get_methods (iclass, &iter))) {
		member = mono_method_get_object (domain, method, iclass);
		mono_array_setref (*methods, i, member);
		member = mono_method_get_object (domain, class->vtable [i + ioffset], class);
		mono_array_setref (*targets, i, member);
		
		i ++;
	}
}