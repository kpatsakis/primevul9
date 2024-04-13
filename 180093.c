param_info_get_type_modifiers (MonoReflectionParameter *param, MonoBoolean optional)
{
	MonoType *type = param->ClassImpl->type;
	MonoClass *member_class = mono_object_class (param->MemberImpl);
	MonoMethod *method = NULL;
	MonoImage *image;
	int pos;
	MonoMethodSignature *sig;

	if (mono_class_is_reflection_method_or_constructor (member_class)) {
		MonoReflectionMethod *rmethod = (MonoReflectionMethod*)param->MemberImpl;
		method = rmethod->method;
	} else if (member_class->image == mono_defaults.corlib && !strcmp ("MonoProperty", member_class->name)) {
		MonoReflectionProperty *prop = (MonoReflectionProperty *)param->MemberImpl;
		if (!(method = prop->property->get))
			method = prop->property->set;
		g_assert (method);	
	} else {
		char *type_name = mono_type_get_full_name (member_class);
		char *msg = g_strdup_printf ("Custom modifiers on a ParamInfo with member %s are not supported", type_name);
		MonoException *ex = mono_get_exception_not_supported  (msg);
		g_free (type_name);
		g_free (msg);
		mono_raise_exception (ex);
	}

	image = method->klass->image;
	pos = param->PositionImpl;
	sig = mono_method_signature (method);
	if (pos == -1)
		type = sig->ret;
	else
		type = sig->params [pos];

	return type_array_from_modifiers (image, type, optional);
}