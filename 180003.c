ves_icall_Type_GetPropertiesByName (MonoReflectionType *type, MonoString *name, guint32 bflags, MonoBoolean ignore_case, MonoReflectionType *reftype)
{
	MonoDomain *domain; 
	static MonoClass *System_Reflection_PropertyInfo;
	MonoClass *startklass, *klass;
	MonoArray *res;
	MonoMethod *method;
	MonoProperty *prop;
	int i, match;
	guint32 flags;
	gchar *propname = NULL;
	int (*compare_func) (const char *s1, const char *s2) = NULL;
	gpointer iter;
	GHashTable *properties;
	MonoPtrArray tmp_array;

	MONO_ARCH_SAVE_REGS;

	mono_ptr_array_init (tmp_array, 8); /*This the average for ASP.NET types*/

	if (!System_Reflection_PropertyInfo)
		System_Reflection_PropertyInfo = mono_class_from_name (
			mono_defaults.corlib, "System.Reflection", "PropertyInfo");

	domain = ((MonoObject *)type)->vtable->domain;
	if (type->type->byref)
		return mono_array_new_cached (domain, System_Reflection_PropertyInfo, 0);
	klass = startklass = mono_class_from_mono_type (type->type);
	if (name != NULL) {
		propname = mono_string_to_utf8 (name);
		compare_func = (ignore_case) ? mono_utf8_strcasecmp : strcmp;
	}

	mono_class_setup_vtable (klass);

	properties = g_hash_table_new (property_hash, (GEqualFunc)property_equal);
handle_parent:
	mono_class_setup_vtable (klass);
	if (klass->exception_type != MONO_EXCEPTION_NONE) {
		g_hash_table_destroy (properties);
		if (name != NULL)
			g_free (propname);
		mono_raise_exception (mono_class_get_exception_for_failure (klass));
	}

	iter = NULL;
	while ((prop = mono_class_get_properties (klass, &iter))) {
		match = 0;
		method = prop->get;
		if (!method)
			method = prop->set;
		if (method)
			flags = method->flags;
		else
			flags = 0;
		if ((prop->get && ((prop->get->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC)) ||
			(prop->set && ((prop->set->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC))) {
			if (bflags & BFLAGS_Public)
				match++;
		} else if (bflags & BFLAGS_NonPublic) {
			if (property_accessor_nonpublic(prop->get, startklass == klass) ||
				property_accessor_nonpublic(prop->set, startklass == klass)) {
				match++;
			}
		}
		if (!match)
			continue;
		match = 0;
		if (flags & METHOD_ATTRIBUTE_STATIC) {
			if (bflags & BFLAGS_Static)
				if ((bflags & BFLAGS_FlattenHierarchy) || (klass == startklass))
					match++;
		} else {
			if (bflags & BFLAGS_Instance)
				match++;
		}

		if (!match)
			continue;
		match = 0;

		if (name != NULL) {
			if (compare_func (propname, prop->name))
				continue;
		}
		
		if (g_hash_table_lookup (properties, prop))
			continue;

		mono_ptr_array_append (tmp_array, mono_property_get_object (domain, startklass, prop));
		
		g_hash_table_insert (properties, prop, prop);
	}
	if ((!(bflags & BFLAGS_DeclaredOnly) && (klass = klass->parent)))
		goto handle_parent;

	g_hash_table_destroy (properties);
	g_free (propname);

	res = mono_array_new_cached (domain, System_Reflection_PropertyInfo, mono_ptr_array_size (tmp_array));
	for (i = 0; i < mono_ptr_array_size (tmp_array); ++i)
		mono_array_setref (res, i, mono_ptr_array_get (tmp_array, i));

	mono_ptr_array_destroy (tmp_array);

	return res;
}