ves_icall_System_MonoType_getFullName (MonoReflectionType *object, gboolean full_name,
				       gboolean assembly_qualified)
{
	MonoDomain *domain = mono_object_domain (object); 
	MonoTypeNameFormat format;
	MonoString *res;
	gchar *name;

	MONO_ARCH_SAVE_REGS;
	if (full_name)
		format = assembly_qualified ?
			MONO_TYPE_NAME_FORMAT_ASSEMBLY_QUALIFIED :
			MONO_TYPE_NAME_FORMAT_FULL_NAME;
	else
		format = MONO_TYPE_NAME_FORMAT_REFLECTION;
 
	name = mono_type_get_name_full (object->type, format);
	if (!name)
		return NULL;

	if (full_name && (object->type->type == MONO_TYPE_VAR || object->type->type == MONO_TYPE_MVAR)) {
		g_free (name);
		return NULL;
	}

	res = mono_string_new (domain, name);
	g_free (name);

	return res;
}