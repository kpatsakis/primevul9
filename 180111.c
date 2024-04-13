ves_icall_get_parameter_info (MonoMethod *method, MonoReflectionMethod *member)
{
	MonoDomain *domain = mono_domain_get (); 

	return mono_param_get_objects_internal (domain, method, member->reftype ? mono_class_from_mono_type (member->reftype->type) : NULL);
}