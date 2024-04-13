ves_icall_Type_GetFields_internal (MonoReflectionType *type, guint32 bflags, MonoReflectionType *reftype)
{
	MonoDomain *domain; 
	MonoClass *startklass, *klass, *refklass;
	MonoArray *res;
	MonoObject *member;
	int i, match;
	gpointer iter;
	MonoClassField *field;
	MonoPtrArray tmp_array;

	MONO_ARCH_SAVE_REGS;

	domain = ((MonoObject *)type)->vtable->domain;
	if (type->type->byref)
		return mono_array_new (domain, mono_defaults.field_info_class, 0);
	klass = startklass = mono_class_from_mono_type (type->type);
	refklass = mono_class_from_mono_type (reftype->type);

	mono_ptr_array_init (tmp_array, 2);
	
handle_parent:	
	if (klass->exception_type != MONO_EXCEPTION_NONE)
		mono_raise_exception (mono_class_get_exception_for_failure (klass));

	iter = NULL;
	while ((field = mono_class_get_fields (klass, &iter))) {
		match = 0;
		if (mono_field_is_deleted (field))
			continue;
		if ((field->type->attrs & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK) == FIELD_ATTRIBUTE_PUBLIC) {
			if (bflags & BFLAGS_Public)
				match++;
		} else if ((klass == startklass) || (field->type->attrs & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK) != FIELD_ATTRIBUTE_PRIVATE) {
			if (bflags & BFLAGS_NonPublic) {
				match++;
			}
		}
		if (!match)
			continue;
		match = 0;
		if (field->type->attrs & FIELD_ATTRIBUTE_STATIC) {
			if (bflags & BFLAGS_Static)
				if ((bflags & BFLAGS_FlattenHierarchy) || (klass == startklass))
					match++;
		} else {
			if (bflags & BFLAGS_Instance)
				match++;
		}

		if (!match)
			continue;
		member = (MonoObject*)mono_field_get_object (domain, refklass, field);
		mono_ptr_array_append (tmp_array, member);
	}
	if (!(bflags & BFLAGS_DeclaredOnly) && (klass = klass->parent))
		goto handle_parent;

	res = mono_array_new_cached (domain, mono_defaults.field_info_class, mono_ptr_array_size (tmp_array));

	for (i = 0; i < mono_ptr_array_size (tmp_array); ++i)
		mono_array_setref (res, i, mono_ptr_array_get (tmp_array, i));

	mono_ptr_array_destroy (tmp_array);

	return res;
}