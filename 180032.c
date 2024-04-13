get_property_type (MonoProperty *prop)
{
	MonoMethodSignature *sig;
	if (prop->get) {
		sig = mono_method_signature (prop->get);
		return sig->ret;
	} else if (prop->set) {
		sig = mono_method_signature (prop->set);
		return sig->params [sig->param_count - 1];
	}
	return NULL;
}