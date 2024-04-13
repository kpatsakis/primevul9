type_from_typename (char *typename)
{
	MonoClass *klass = NULL;	/* assignment to shut GCC warning up */

	if (!strcmp (typename, "int"))
		klass = mono_defaults.int_class;
	else if (!strcmp (typename, "ptr"))
		klass = mono_defaults.int_class;
	else if (!strcmp (typename, "void"))
		klass = mono_defaults.void_class;
	else if (!strcmp (typename, "int32"))
		klass = mono_defaults.int32_class;
	else if (!strcmp (typename, "uint32"))
		klass = mono_defaults.uint32_class;
	else if (!strcmp (typename, "int8"))
		klass = mono_defaults.sbyte_class;
	else if (!strcmp (typename, "uint8"))
		klass = mono_defaults.byte_class;
	else if (!strcmp (typename, "int16"))
		klass = mono_defaults.int16_class;
	else if (!strcmp (typename, "uint16"))
		klass = mono_defaults.uint16_class;
	else if (!strcmp (typename, "long"))
		klass = mono_defaults.int64_class;
	else if (!strcmp (typename, "ulong"))
		klass = mono_defaults.uint64_class;
	else if (!strcmp (typename, "float"))
		klass = mono_defaults.single_class;
	else if (!strcmp (typename, "double"))
		klass = mono_defaults.double_class;
	else if (!strcmp (typename, "object"))
		klass = mono_defaults.object_class;
	else if (!strcmp (typename, "obj"))
		klass = mono_defaults.object_class;
	else if (!strcmp (typename, "string"))
		klass = mono_defaults.string_class;
	else if (!strcmp (typename, "bool"))
		klass = mono_defaults.boolean_class;
	else if (!strcmp (typename, "boolean"))
		klass = mono_defaults.boolean_class;
	else {
		g_error ("%s", typename);
		g_assert_not_reached ();
	}
	return &klass->byval_arg;
}