type_array_from_modifiers (MonoImage *image, MonoType *type, int optional)
{
	MonoArray *res;
	int i, count = 0;
	for (i = 0; i < type->num_mods; ++i) {
		if ((optional && !type->modifiers [i].required) || (!optional && type->modifiers [i].required))
			count++;
	}
	if (!count)
		return NULL;
	res = mono_array_new (mono_domain_get (), mono_defaults.systemtype_class, count);
	count = 0;
	for (i = 0; i < type->num_mods; ++i) {
		if ((optional && !type->modifiers [i].required) || (!optional && type->modifiers [i].required)) {
			MonoClass *klass = mono_class_get (image, type->modifiers [i].token);
			mono_array_setref (res, count, mono_type_get_object (mono_domain_get (), &klass->byval_arg));
			count++;
		}
	}
	return res;
}