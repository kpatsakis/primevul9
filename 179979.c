ves_icall_System_Enum_ToObject (MonoReflectionType *enumType, MonoObject *value)
{
	MonoDomain *domain; 
	MonoClass *enumc, *objc;
	MonoObject *res;
	MonoType *etype;
	guint64 val;
	
	MONO_ARCH_SAVE_REGS;

	MONO_CHECK_ARG_NULL (enumType);
	MONO_CHECK_ARG_NULL (value);

	domain = mono_object_domain (enumType); 
	enumc = mono_class_from_mono_type (enumType->type);
	objc = value->vtable->klass;

	if (!enumc->enumtype)
		mono_raise_exception (mono_get_exception_argument ("enumType", "Type provided must be an Enum."));
	if (!((objc->enumtype) || (objc->byval_arg.type >= MONO_TYPE_I1 && objc->byval_arg.type <= MONO_TYPE_U8)))
		mono_raise_exception (mono_get_exception_argument ("value", "The value passed in must be an enum base or an underlying type for an enum, such as an Int32."));

	etype = mono_class_enum_basetype (enumc);
	if (!etype)
		/* MS throws this for typebuilders */
		mono_raise_exception (mono_get_exception_argument ("Type must be a type provided by the runtime.", "enumType"));

	res = mono_object_new (domain, enumc);
	val = read_enum_value ((char *)value + sizeof (MonoObject), objc->enumtype? mono_class_enum_basetype (objc)->type: objc->byval_arg.type);
	write_enum_value ((char *)res + sizeof (MonoObject), etype->type, val);

	return res;
}