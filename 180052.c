ves_icall_System_Reflection_FieldInfo_GetTypeModifiers (MonoReflectionField *field, MonoBoolean optional)
{
	MonoType *type = field->field->type;

	return type_array_from_modifiers (field->field->parent->image, type, optional);
}