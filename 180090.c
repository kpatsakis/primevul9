ves_icall_Type_GetInterfaces (MonoReflectionType* type)
{
	MonoError error;
	MonoDomain *domain = mono_object_domain (type); 
	MonoArray *intf;
	GPtrArray *ifaces = NULL;
	int i;
	MonoClass *class = mono_class_from_mono_type (type->type);
	MonoClass *parent;
	MonoBitSet *slots;
	MonoGenericContext *context = NULL;

	MONO_ARCH_SAVE_REGS;

	if (class->generic_class && class->generic_class->context.class_inst->is_open) {
		context = mono_class_get_context (class);
		class = class->generic_class->container_class;
	}

	mono_class_setup_vtable (class);

	slots = mono_bitset_new (class->max_interface_id + 1, 0);

	for (parent = class; parent; parent = parent->parent) {
		GPtrArray *tmp_ifaces = mono_class_get_implemented_interfaces (parent, &error);
		if (!mono_error_ok (&error)) {
			mono_bitset_free (slots);
			mono_error_raise_exception (&error);
			return NULL;
		} else if (tmp_ifaces) {
			for (i = 0; i < tmp_ifaces->len; ++i) {
				MonoClass *ic = g_ptr_array_index (tmp_ifaces, i);

				if (mono_bitset_test (slots, ic->interface_id))
					continue;

				mono_bitset_set (slots, ic->interface_id);
				if (ifaces == NULL)
					ifaces = g_ptr_array_new ();
				g_ptr_array_add (ifaces, ic);
			}
			g_ptr_array_free (tmp_ifaces, TRUE);
		}
	}
	mono_bitset_free (slots);

	if (!ifaces)
		return mono_array_new_cached (domain, mono_defaults.monotype_class, 0);
		
	intf = mono_array_new_cached (domain, mono_defaults.monotype_class, ifaces->len);
	for (i = 0; i < ifaces->len; ++i) {
		MonoClass *ic = g_ptr_array_index (ifaces, i);
		MonoType *ret = &ic->byval_arg, *inflated = NULL;
		if (context && ic->generic_class && ic->generic_class->context.class_inst->is_open)
			inflated = ret = mono_class_inflate_generic_type (ret, context);
		
		mono_array_setref (intf, i, mono_type_get_object (domain, ret));
		if (inflated)
			mono_metadata_free_type (inflated);
	}
	g_ptr_array_free (ifaces, TRUE);

	return intf;
}