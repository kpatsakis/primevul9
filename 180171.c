ves_icall_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue (MonoObject *obj)
{
	MONO_ARCH_SAVE_REGS;

	if ((obj == NULL) || (! (obj->vtable->klass->valuetype)))
		return obj;
	else
		return mono_object_clone (obj);
}