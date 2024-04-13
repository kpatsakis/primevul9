ves_icall_MonoField_GetFieldOffset (MonoReflectionField *field)
{
	return field->field->offset - sizeof (MonoObject);
}