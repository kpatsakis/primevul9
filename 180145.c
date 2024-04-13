ves_icall_MonoType_GetEvent (MonoReflectionType *type, MonoString *name, guint32 bflags)
{
	MonoDomain *domain;
	MonoClass *klass, *startklass;
	gpointer iter;
	MonoEvent *event;
	MonoMethod *method;
	gchar *event_name;
	int (*compare_func) (const char *s1, const char *s2) = NULL;
	MONO_ARCH_SAVE_REGS;

	event_name = mono_string_to_utf8 (name);
	if (type->type->byref)
		return NULL;
	klass = startklass = mono_class_from_mono_type (type->type);
	domain = mono_object_domain (type);

	compare_func = (bflags & BFLAGS_IgnoreCase) ? mono_utf8_strcasecmp : strcmp;

  handle_parent:
	if (klass->exception_type != MONO_EXCEPTION_NONE)
		mono_raise_exception (mono_class_get_exception_for_failure (klass));

	iter = NULL;
	while ((event = mono_class_get_events (klass, &iter))) {
		if (compare_func (event->name, event_name))
			continue;

		method = event->add;
		if (!method)
			method = event->remove;
		if (!method)
			method = event->raise;
		if (method) {
			if ((method->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC) {
				if (!(bflags & BFLAGS_Public))
					continue;
			} else {
				if (!(bflags & BFLAGS_NonPublic))
					continue;
				if ((klass != startklass) && (method->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PRIVATE)
					continue;
			}

			if (method->flags & METHOD_ATTRIBUTE_STATIC) {
				if (!(bflags & BFLAGS_Static))
					continue;
				if (!(bflags & BFLAGS_FlattenHierarchy) && (klass != startklass))
					continue;
			} else {
				if (!(bflags & BFLAGS_Instance))
					continue;
			}
		} else 
			if (!(bflags & BFLAGS_NonPublic))
				continue;
		
		g_free (event_name);
		return mono_event_get_object (domain, startklass, event);
	}

	if (!(bflags & BFLAGS_DeclaredOnly) && (klass = klass->parent))
		goto handle_parent;

	g_free (event_name);
	return NULL;
}