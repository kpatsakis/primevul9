ves_icall_MonoField_GetParentType (MonoReflectionField *field, MonoBoolean declaring)
{
	MonoClass *parent;
	MONO_ARCH_SAVE_REGS;

	parent = declaring? field->field->parent: field->klass;

	return mono_type_get_object (mono_object_domain (field), &parent->byval_arg);
}