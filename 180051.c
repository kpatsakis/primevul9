ves_icall_Type_GetField (MonoReflectionType *type, MonoString *name, guint32 bflags)
{
	MonoDomain *domain; 
	MonoClass *startklass, *klass;
	int match;
	MonoClassField *field;
	gpointer iter;
	char *utf8_name;
	int (*compare_func) (const char *s1, const char *s2) = NULL;
	domain = ((MonoObject *)type)->vtable->domain;
	klass = startklass = mono_class_from_mono_type (type->type);

	MONO_ARCH_SAVE_REGS;

	if (!name)
		mono_raise_exception (mono_get_exception_argument_null ("name"));
	if (type->type->byref)
		return NULL;

	compare_func = (bflags & BFLAGS_IgnoreCase) ? mono_utf8_strcasecmp : strcmp;

handle_parent:
	if (klass->exception_type != MONO_EXCEPTION_NONE)
		mono_raise_exception (mono_class_get_exception_for_failure (klass));

	iter = NULL;
	while ((field = mono_class_get_fields (klass, &iter))) {
		match = 0;

		if (field->type == NULL)
			continue;
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
		
		utf8_name = mono_string_to_utf8 (name);

		if (compare_func (mono_field_get_name (field), utf8_name)) {
			g_free (utf8_name);
			continue;
		}
		g_free (utf8_name);
		
		return mono_field_get_object (domain, klass, field);
	}
	if (!(bflags & BFLAGS_DeclaredOnly) && (klass = klass->parent))
		goto handle_parent;

	return NULL;
}