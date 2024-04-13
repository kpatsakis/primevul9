mono_module_get_types (MonoDomain *domain, MonoImage *image, MonoArray **exceptions, MonoBoolean exportedOnly)
{
	MonoArray *res;
	MonoClass *klass;
	MonoTableInfo *tdef = &image->tables [MONO_TABLE_TYPEDEF];
	int i, count;
	guint32 attrs, visibility;

	/* we start the count from 1 because we skip the special type <Module> */
	if (exportedOnly) {
		count = 0;
		for (i = 1; i < tdef->rows; ++i) {
			attrs = mono_metadata_decode_row_col (tdef, i, MONO_TYPEDEF_FLAGS);
			visibility = attrs & TYPE_ATTRIBUTE_VISIBILITY_MASK;
			if (visibility == TYPE_ATTRIBUTE_PUBLIC || visibility == TYPE_ATTRIBUTE_NESTED_PUBLIC)
				count++;
		}
	} else {
		count = tdef->rows - 1;
	}
	res = mono_array_new (domain, mono_defaults.monotype_class, count);
	*exceptions = mono_array_new (domain, mono_defaults.exception_class, count);
	count = 0;
	for (i = 1; i < tdef->rows; ++i) {
		attrs = mono_metadata_decode_row_col (tdef, i, MONO_TYPEDEF_FLAGS);
		visibility = attrs & TYPE_ATTRIBUTE_VISIBILITY_MASK;
		if (!exportedOnly || (visibility == TYPE_ATTRIBUTE_PUBLIC || visibility == TYPE_ATTRIBUTE_NESTED_PUBLIC)) {
			klass = mono_class_get (image, (i + 1) | MONO_TOKEN_TYPE_DEF);
			if (klass) {
				mono_array_setref (res, count, mono_type_get_object (domain, &klass->byval_arg));
			} else {
				MonoLoaderError *error;
				MonoException *ex;
				
				error = mono_loader_get_last_error ();
				g_assert (error != NULL);
	
				ex = mono_loader_error_prepare_exception (error);
				mono_array_setref (*exceptions, count, ex);
			}
			if (mono_loader_get_last_error ())
				mono_loader_clear_error ();
			count++;
		}
	}
	
	return res;
}