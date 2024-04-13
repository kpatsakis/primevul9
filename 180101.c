ves_icall_System_Reflection_Assembly_GetTypes (MonoReflectionAssembly *assembly, MonoBoolean exportedOnly)
{
	MonoArray *res = NULL;
	MonoArray *exceptions = NULL;
	MonoImage *image = NULL;
	MonoTableInfo *table = NULL;
	MonoDomain *domain;
	GList *list = NULL;
	int i, len, ex_count;

	MONO_ARCH_SAVE_REGS;

	domain = mono_object_domain (assembly);

	g_assert (!assembly->assembly->dynamic);
	image = assembly->assembly->image;
	table = &image->tables [MONO_TABLE_FILE];
	res = mono_module_get_types (domain, image, &exceptions, exportedOnly);

	/* Append data from all modules in the assembly */
	for (i = 0; i < table->rows; ++i) {
		if (!(mono_metadata_decode_row_col (table, i, MONO_FILE_FLAGS) & FILE_CONTAINS_NO_METADATA)) {
			MonoImage *loaded_image = mono_assembly_load_module (image->assembly, i + 1);
			if (loaded_image) {
				MonoArray *ex2;
				MonoArray *res2 = mono_module_get_types (domain, loaded_image, &ex2, exportedOnly);
				/* Append the new types to the end of the array */
				if (mono_array_length (res2) > 0) {
					guint32 len1, len2;
					MonoArray *res3, *ex3;

					len1 = mono_array_length (res);
					len2 = mono_array_length (res2);

					res3 = mono_array_new (domain, mono_defaults.monotype_class, len1 + len2);
					mono_array_memcpy_refs (res3, 0, res, 0, len1);
					mono_array_memcpy_refs (res3, len1, res2, 0, len2);
					res = res3;

					ex3 = mono_array_new (domain, mono_defaults.monotype_class, len1 + len2);
					mono_array_memcpy_refs (ex3, 0, exceptions, 0, len1);
					mono_array_memcpy_refs (ex3, len1, ex2, 0, len2);
					exceptions = ex3;
				}
			}
		}
	}

	/* the ReflectionTypeLoadException must have all the types (Types property), 
	 * NULL replacing types which throws an exception. The LoaderException must
	 * contain all exceptions for NULL items.
	 */

	len = mono_array_length (res);

	ex_count = 0;
	for (i = 0; i < len; i++) {
		MonoReflectionType *t = mono_array_get (res, gpointer, i);
		MonoClass *klass;

		if (t) {
			klass = mono_type_get_class (t->type);
			if ((klass != NULL) && klass->exception_type) {
				/* keep the class in the list */
				list = g_list_append (list, klass);
				/* and replace Type with NULL */
				mono_array_setref (res, i, NULL);
			}
		} else {
			ex_count ++;
		}
	}

	if (list || ex_count) {
		GList *tmp = NULL;
		MonoException *exc = NULL;
		MonoArray *exl = NULL;
		int j, length = g_list_length (list) + ex_count;

		mono_loader_clear_error ();

		exl = mono_array_new (domain, mono_defaults.exception_class, length);
		/* Types for which mono_class_get () succeeded */
		for (i = 0, tmp = list; tmp; i++, tmp = tmp->next) {
			MonoException *exc = mono_class_get_exception_for_failure (tmp->data);
			mono_array_setref (exl, i, exc);
		}
		/* Types for which it don't */
		for (j = 0; j < mono_array_length (exceptions); ++j) {
			MonoException *exc = mono_array_get (exceptions, MonoException*, j);
			if (exc) {
				g_assert (i < length);
				mono_array_setref (exl, i, exc);
				i ++;
			}
		}
		g_list_free (list);
		list = NULL;

		exc = mono_get_exception_reflection_type_load (res, exl);
		mono_loader_clear_error ();
		mono_raise_exception (exc);
	}
		
	return res;
}