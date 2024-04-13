ves_icall_MonoGenericMethod_get_ReflectedType (MonoReflectionGenericMethod *rmethod)
{
	MonoMethod *method = rmethod->method.method;

	return mono_type_get_object (mono_object_domain (rmethod), &method->klass->byval_arg);
}