ves_icall_MonoMethod_GetDllImportAttribute (MonoMethod *method)
{
	static MonoClass *DllImportAttributeClass = NULL;
	MonoDomain *domain = mono_domain_get ();
	MonoReflectionDllImportAttribute *attr;
	MonoImage *image = method->klass->image;
	MonoMethodPInvoke *piinfo = (MonoMethodPInvoke *)method;
	MonoTableInfo *tables = image->tables;
	MonoTableInfo *im = &tables [MONO_TABLE_IMPLMAP];
	MonoTableInfo *mr = &tables [MONO_TABLE_MODULEREF];
	guint32 im_cols [MONO_IMPLMAP_SIZE];
	guint32 scope_token;
	const char *import = NULL;
	const char *scope = NULL;
	guint32 flags;

	if (!(method->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL))
		return NULL;

	if (!DllImportAttributeClass) {
		DllImportAttributeClass = 
			mono_class_from_name (mono_defaults.corlib,
								  "System.Runtime.InteropServices", "DllImportAttribute");
		g_assert (DllImportAttributeClass);
	}
														
	if (method->klass->image->dynamic) {
		MonoReflectionMethodAux *method_aux = 
			g_hash_table_lookup (
									  ((MonoDynamicImage*)method->klass->image)->method_aux_hash, method);
		if (method_aux) {
			import = method_aux->dllentry;
			scope = method_aux->dll;
		}

		if (!import || !scope) {
			mono_raise_exception (mono_get_exception_argument ("method", "System.Reflection.Emit method with invalid pinvoke information"));
			return NULL;
		}
	}
	else {
		if (piinfo->implmap_idx) {
			mono_metadata_decode_row (im, piinfo->implmap_idx - 1, im_cols, MONO_IMPLMAP_SIZE);
			
			piinfo->piflags = im_cols [MONO_IMPLMAP_FLAGS];
			import = mono_metadata_string_heap (image, im_cols [MONO_IMPLMAP_NAME]);
			scope_token = mono_metadata_decode_row_col (mr, im_cols [MONO_IMPLMAP_SCOPE] - 1, MONO_MODULEREF_NAME);
			scope = mono_metadata_string_heap (image, scope_token);
		}
	}
	flags = piinfo->piflags;
	
	attr = (MonoReflectionDllImportAttribute*)mono_object_new (domain, DllImportAttributeClass);

	MONO_OBJECT_SETREF (attr, dll, mono_string_new (domain, scope));
	MONO_OBJECT_SETREF (attr, entry_point, mono_string_new (domain, import));
	attr->call_conv = (flags & 0x700) >> 8;
	attr->charset = ((flags & 0x6) >> 1) + 1;
	if (attr->charset == 1)
		attr->charset = 2;
	attr->exact_spelling = (flags & 0x1) != 0;
	attr->set_last_error = (flags & 0x40) != 0;
	attr->best_fit_mapping = (flags & 0x30) == 0x10;
	attr->throw_on_unmappable = (flags & 0x3000) == 0x1000;
	attr->preserve_sig = FALSE;

	return attr;
}