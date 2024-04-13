ves_icall_Type_GetMethodsByName (MonoReflectionType *type, MonoString *name, guint32 bflags, MonoBoolean ignore_case, MonoReflectionType *reftype)
{
	static MonoClass *MethodInfo_array;
	MonoDomain *domain; 
	MonoClass *startklass, *klass, *refklass;
	MonoArray *res;
	MonoMethod *method;
	gpointer iter;
	MonoObject *member;
	int i, len, match, nslots;
	/*FIXME, use MonoBitSet*/
	guint32 method_slots_default [8];
	guint32 *method_slots = NULL;
	gchar *mname = NULL;
	int (*compare_func) (const char *s1, const char *s2) = NULL;
	MonoVTable *array_vtable;
	MonoException *ex;
	MonoPtrArray tmp_array;

	MONO_ARCH_SAVE_REGS;

	mono_ptr_array_init (tmp_array, 4);

	if (!MethodInfo_array) {
		MonoClass *klass = mono_array_class_get (mono_defaults.method_info_class, 1);
		mono_memory_barrier ();
		MethodInfo_array = klass;
	}

	domain = ((MonoObject *)type)->vtable->domain;
	array_vtable = mono_class_vtable_full (domain, MethodInfo_array, TRUE);
	if (type->type->byref)
		return mono_array_new_specific (array_vtable, 0);
	klass = startklass = mono_class_from_mono_type (type->type);
	refklass = mono_class_from_mono_type (reftype->type);
	len = 0;
	if (name != NULL) {
		mname = mono_string_to_utf8 (name);
		compare_func = (ignore_case) ? mono_utf8_strcasecmp : strcmp;
	}

	/* An optimization for calls made from Delegate:CreateDelegate () */
	if (klass->delegate && mname && !strcmp (mname, "Invoke") && (bflags == (BFLAGS_Public | BFLAGS_Static | BFLAGS_Instance))) {
		method = mono_get_delegate_invoke (klass);
		if (mono_loader_get_last_error ())
			goto loader_error;

		member = (MonoObject*)mono_method_get_object (domain, method, refklass);

		res = mono_array_new_specific (array_vtable, 1);
		mono_array_setref (res, 0, member);
		g_free (mname);
		return res;
	}

	mono_class_setup_vtable (klass);
	if (klass->exception_type != MONO_EXCEPTION_NONE || mono_loader_get_last_error ())
		goto loader_error;

	if (is_generic_parameter (type->type))
		nslots = mono_class_get_vtable_size (klass->parent);
	else
		nslots = MONO_CLASS_IS_INTERFACE (klass) ? mono_class_num_methods (klass) : mono_class_get_vtable_size (klass);
	if (nslots >= sizeof (method_slots_default) * 8) {
		method_slots = g_new0 (guint32, nslots / 32 + 1);
	} else {
		method_slots = method_slots_default;
		memset (method_slots, 0, sizeof (method_slots_default));
	}
handle_parent:
	mono_class_setup_vtable (klass);
	if (klass->exception_type != MONO_EXCEPTION_NONE || mono_loader_get_last_error ())
		goto loader_error;		

	iter = NULL;
	while ((method = mono_class_get_methods (klass, &iter))) {
		match = 0;
		if (method->slot != -1) {
			g_assert (method->slot < nslots);
			if (method_slots [method->slot >> 5] & (1 << (method->slot & 0x1f)))
				continue;
			if (!(method->flags & METHOD_ATTRIBUTE_NEW_SLOT))
				method_slots [method->slot >> 5] |= 1 << (method->slot & 0x1f);
		}

		if (method->name [0] == '.' && (strcmp (method->name, ".ctor") == 0 || strcmp (method->name, ".cctor") == 0))
			continue;
		if ((method->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC) {
			if (bflags & BFLAGS_Public)
				match++;
		} else if ((bflags & BFLAGS_NonPublic) && method_nonpublic (method, (klass == startklass))) {
				match++;
		}
		if (!match)
			continue;
		match = 0;
		if (method->flags & METHOD_ATTRIBUTE_STATIC) {
			if (bflags & BFLAGS_Static)
				if ((bflags & BFLAGS_FlattenHierarchy) || (klass == startklass))
					match++;
		} else {
			if (bflags & BFLAGS_Instance)
				match++;
		}

		if (!match)
			continue;

		if (name != NULL) {
			if (compare_func (mname, method->name))
				continue;
		}
		
		match = 0;
		
		member = (MonoObject*)mono_method_get_object (domain, method, refklass);
		
		mono_ptr_array_append (tmp_array, member);
	}
	if (!(bflags & BFLAGS_DeclaredOnly) && (klass = klass->parent))
		goto handle_parent;

	g_free (mname);
	if (method_slots != method_slots_default)
		g_free (method_slots);

	res = mono_array_new_specific (array_vtable, mono_ptr_array_size (tmp_array));

	for (i = 0; i < mono_ptr_array_size (tmp_array); ++i)
		mono_array_setref (res, i, mono_ptr_array_get (tmp_array, i));

	mono_ptr_array_destroy (tmp_array);
	return res;

loader_error:
	g_free (mname);
	if (method_slots != method_slots_default)
		g_free (method_slots);
	mono_ptr_array_destroy (tmp_array);
	if (klass->exception_type != MONO_EXCEPTION_NONE) {
		ex = mono_class_get_exception_for_failure (klass);
	} else {
		ex = mono_loader_error_prepare_exception (mono_loader_get_last_error ());
		mono_loader_clear_error ();
	}
	mono_raise_exception (ex);
	return NULL;
}