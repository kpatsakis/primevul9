ves_icall_Type_GetEvents_internal (MonoReflectionType *type, guint32 bflags, MonoReflectionType *reftype)
{
	MonoDomain *domain; 
	static MonoClass *System_Reflection_EventInfo;
	MonoClass *startklass, *klass;
	MonoArray *res;
	MonoMethod *method;
	MonoEvent *event;
	int i, match;
	gpointer iter;
	
	MonoPtrArray tmp_array;

	MONO_ARCH_SAVE_REGS;

	mono_ptr_array_init (tmp_array, 4);

	if (!System_Reflection_EventInfo)
		System_Reflection_EventInfo = mono_class_from_name (
			mono_defaults.corlib, "System.Reflection", "EventInfo");

	domain = mono_object_domain (type);
	if (type->type->byref)
		return mono_array_new_cached (domain, System_Reflection_EventInfo, 0);
	klass = startklass = mono_class_from_mono_type (type->type);

handle_parent:	
	if (klass->exception_type != MONO_EXCEPTION_NONE)
		mono_raise_exception (mono_class_get_exception_for_failure (klass));

	iter = NULL;
	while ((event = mono_class_get_events (klass, &iter))) {
		match = 0;
		method = event->add;
		if (!method)
			method = event->remove;
		if (!method)
			method = event->raise;
		if (method) {
			if ((method->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC) {
				if (bflags & BFLAGS_Public)
					match++;
			} else if ((klass == startklass) || (method->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) != METHOD_ATTRIBUTE_PRIVATE) {
				if (bflags & BFLAGS_NonPublic)
					match++;
			}
		}
		else
			if (bflags & BFLAGS_NonPublic)
				match ++;
		if (!match)
			continue;
		match = 0;
		if (method) {
			if (method->flags & METHOD_ATTRIBUTE_STATIC) {
				if (bflags & BFLAGS_Static)
					if ((bflags & BFLAGS_FlattenHierarchy) || (klass == startklass))
						match++;
			} else {
				if (bflags & BFLAGS_Instance)
					match++;
			}
		}
		else
			if (bflags & BFLAGS_Instance)
				match ++;
		if (!match)
			continue;
		mono_ptr_array_append (tmp_array, mono_event_get_object (domain, startklass, event));
	}
	if (!(bflags & BFLAGS_DeclaredOnly) && (klass = klass->parent))
		goto handle_parent;

	res = mono_array_new_cached (domain, System_Reflection_EventInfo, mono_ptr_array_size (tmp_array));

	for (i = 0; i < mono_ptr_array_size (tmp_array); ++i)
		mono_array_setref (res, i, mono_ptr_array_get (tmp_array, i));

	mono_ptr_array_destroy (tmp_array);

	return res;
}