ves_icall_Type_GetNestedTypes (MonoReflectionType *type, guint32 bflags)
{
	MonoDomain *domain; 
	MonoClass *klass;
	MonoArray *res;
	MonoObject *member;
	int i, match;
	MonoClass *nested;
	gpointer iter;
	MonoPtrArray tmp_array;

	MONO_ARCH_SAVE_REGS;

	domain = ((MonoObject *)type)->vtable->domain;
	if (type->type->byref)
		return mono_array_new (domain, mono_defaults.monotype_class, 0);
	klass = mono_class_from_mono_type (type->type);
	if (klass->exception_type != MONO_EXCEPTION_NONE)
		mono_raise_exception (mono_class_get_exception_for_failure (klass));

	/*
	 * If a nested type is generic, return its generic type definition.
	 * Note that this means that the return value is essentially the set
	 * of nested types of the generic type definition of @klass.
	 *
	 * A note in MSDN claims that a generic type definition can have
	 * nested types that aren't generic.  In any case, the container of that
	 * nested type would be the generic type definition.
	 */
	if (klass->generic_class)
		klass = klass->generic_class->container_class;

	mono_ptr_array_init (tmp_array, 1);
	iter = NULL;
	while ((nested = mono_class_get_nested_types (klass, &iter))) {
		match = 0;
		if ((nested->flags & TYPE_ATTRIBUTE_VISIBILITY_MASK) == TYPE_ATTRIBUTE_NESTED_PUBLIC) {
			if (bflags & BFLAGS_Public)
				match++;
		} else {
			if (bflags & BFLAGS_NonPublic)
				match++;
		}
		if (!match)
			continue;
		member = (MonoObject*)mono_type_get_object (domain, &nested->byval_arg);
		mono_ptr_array_append (tmp_array, member);
	}

	res = mono_array_new_cached (domain, mono_defaults.monotype_class, mono_ptr_array_size (tmp_array));

	for (i = 0; i < mono_ptr_array_size (tmp_array); ++i)
		mono_array_setref (res, i, mono_ptr_array_get (tmp_array, i));

	mono_ptr_array_destroy (tmp_array);

	return res;
}