ves_icall_System_Array_CreateInstanceImpl64 (MonoReflectionType *type, MonoArray *lengths, MonoArray *bounds)
{
	MonoClass *aklass;
	MonoArray *array;
	mono_array_size_t *sizes, i;
	gboolean bounded = FALSE;

	MONO_ARCH_SAVE_REGS;

	MONO_CHECK_ARG_NULL (type);
	MONO_CHECK_ARG_NULL (lengths);

	MONO_CHECK_ARG (lengths, mono_array_length (lengths) > 0);
	if (bounds)
		MONO_CHECK_ARG (bounds, mono_array_length (lengths) == mono_array_length (bounds));

	for (i = 0; i < mono_array_length (lengths); i++) 
		if ((mono_array_get (lengths, gint64, i) < 0) ||
		    (mono_array_get (lengths, gint64, i) > MONO_ARRAY_MAX_INDEX))
			mono_raise_exception (mono_get_exception_argument_out_of_range (NULL));

	if (bounds && (mono_array_length (bounds) == 1) && (mono_array_get (bounds, gint64, 0) != 0))
		/* vectors are not the same as one dimensional arrays with no-zero bounds */
		bounded = TRUE;
	else
		bounded = FALSE;

	aklass = mono_bounded_array_class_get (mono_class_from_mono_type (type->type), mono_array_length (lengths), bounded);

	sizes = alloca (aklass->rank * sizeof(mono_array_size_t) * 2);
	for (i = 0; i < aklass->rank; ++i) {
		sizes [i] = mono_array_get (lengths, guint64, i);
		if (bounds)
			sizes [i + aklass->rank] = (mono_array_size_t) mono_array_get (bounds, guint64, i);
		else
			sizes [i + aklass->rank] = 0;
	}

	array = mono_array_new_full (mono_object_domain (type), aklass, sizes, sizes + aklass->rank);

	return array;
}