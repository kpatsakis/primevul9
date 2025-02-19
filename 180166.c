custom_attrs_get_by_type (MonoObject *obj, MonoReflectionType *attr_type)
{
	MonoArray *res = mono_reflection_get_custom_attrs_by_type (obj, attr_type ? mono_class_from_mono_type (attr_type->type) : NULL);

	if (mono_loader_get_last_error ()) {
		mono_raise_exception (mono_loader_error_prepare_exception (mono_loader_get_last_error ()));
		g_assert_not_reached ();
		/* Not reached */
		return NULL;
	} else {
		return res;
	}
}