ves_icall_System_Object_GetType (MonoObject *obj)
{
	MONO_ARCH_SAVE_REGS;

	if (obj->vtable->klass != mono_defaults.transparent_proxy_class)
		return mono_type_get_object (mono_object_domain (obj), &obj->vtable->klass->byval_arg);
	else
		return mono_type_get_object (mono_object_domain (obj), &((MonoTransparentProxy*)obj)->remote_class->proxy_class->byval_arg);
}