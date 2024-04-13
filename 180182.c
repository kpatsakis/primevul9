ves_icall_EnumBuilder_setup_enum_type (MonoReflectionType *enumtype,
									   MonoReflectionType *t)
{
	enumtype->type = t->type;
}