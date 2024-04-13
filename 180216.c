ves_icall_type_IsInstanceOfType (MonoReflectionType *type, MonoObject *obj)
{
	MonoClass *klass = mono_class_from_mono_type (type->type);
	return mono_object_isinst (obj, klass) != NULL;
}