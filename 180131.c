ves_icall_MonoType_GetArrayRank (MonoReflectionType *type)
{
	MonoClass *class;

	if (type->type->type != MONO_TYPE_ARRAY && type->type->type != MONO_TYPE_SZARRAY)
		mono_raise_exception (mono_get_exception_argument ("type", "Type must be an array type"));

	class = mono_class_from_mono_type (type->type);
	return class->rank;
}