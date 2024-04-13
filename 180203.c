ves_icall_System_Reflection_Module_ResolveMemberToken (MonoImage *image, guint32 token, MonoArray *type_args, MonoArray *method_args, MonoResolveTokenError *error)
{
	int table = mono_metadata_token_table (token);

	*error = ResolveTokenError_Other;

	switch (table) {
	case MONO_TABLE_TYPEDEF:
	case MONO_TABLE_TYPEREF:
	case MONO_TABLE_TYPESPEC: {
		MonoType *t = ves_icall_System_Reflection_Module_ResolveTypeToken (image, token, type_args, method_args, error);
		if (t)
			return (MonoObject*)mono_type_get_object (mono_domain_get (), t);
		else
			return NULL;
	}
	case MONO_TABLE_METHOD:
	case MONO_TABLE_METHODSPEC: {
		MonoMethod *m = ves_icall_System_Reflection_Module_ResolveMethodToken (image, token, type_args, method_args, error);
		if (m)
			return (MonoObject*)mono_method_get_object (mono_domain_get (), m, m->klass);
		else
			return NULL;
	}		
	case MONO_TABLE_FIELD: {
		MonoClassField *f = ves_icall_System_Reflection_Module_ResolveFieldToken (image, token, type_args, method_args, error);
		if (f)
			return (MonoObject*)mono_field_get_object (mono_domain_get (), f->parent, f);
		else
			return NULL;
	}
	case MONO_TABLE_MEMBERREF:
		if (mono_metadata_memberref_is_method (image, token)) {
			MonoMethod *m = ves_icall_System_Reflection_Module_ResolveMethodToken (image, token, type_args, method_args, error);
			if (m)
				return (MonoObject*)mono_method_get_object (mono_domain_get (), m, m->klass);
			else
				return NULL;
		}
		else {
			MonoClassField *f = ves_icall_System_Reflection_Module_ResolveFieldToken (image, token, type_args, method_args, error);
			if (f)
				return (MonoObject*)mono_field_get_object (mono_domain_get (), f->parent, f);
			else
				return NULL;
		}
		break;

	default:
		*error = ResolveTokenError_BadTable;
	}

	return NULL;
}