mono_type_type_from_obj (MonoReflectionType *mtype, MonoObject *obj)
{
	MONO_ARCH_SAVE_REGS;

	mtype->type = &obj->vtable->klass->byval_arg;
	g_assert (mtype->type->type);
}