ves_icall_get_event_info (MonoReflectionMonoEvent *event, MonoEventInfo *info)
{
	MonoDomain *domain = mono_object_domain (event); 

	MONO_ARCH_SAVE_REGS;

	MONO_STRUCT_SETREF (info, reflected_type, mono_type_get_object (domain, &event->klass->byval_arg));
	MONO_STRUCT_SETREF (info, declaring_type, mono_type_get_object (domain, &event->event->parent->byval_arg));

	MONO_STRUCT_SETREF (info, name, mono_string_new (domain, event->event->name));
	info->attrs = event->event->attrs;
	MONO_STRUCT_SETREF (info, add_method, event->event->add ? mono_method_get_object (domain, event->event->add, NULL): NULL);
	MONO_STRUCT_SETREF (info, remove_method, event->event->remove ? mono_method_get_object (domain, event->event->remove, NULL): NULL);
	MONO_STRUCT_SETREF (info, raise_method, event->event->raise ? mono_method_get_object (domain, event->event->raise, NULL): NULL);

	if (event->event->other) {
		int i, n = 0;
		while (event->event->other [n])
			n++;
		MONO_STRUCT_SETREF (info, other_methods, mono_array_new (domain, mono_defaults.method_info_class, n));

		for (i = 0; i < n; i++)
			mono_array_setref (info->other_methods, i, mono_method_get_object (domain, event->event->other [i], NULL));
	}		
}