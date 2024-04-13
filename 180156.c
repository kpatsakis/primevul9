ves_icall_Type_make_array_type (MonoReflectionType *type, int rank)
{
	MonoClass *klass, *aklass;

	MONO_ARCH_SAVE_REGS;

	klass = mono_class_from_mono_type (type->type);
	check_for_invalid_type (klass);

	if (rank == 0) //single dimentional array
		aklass = mono_array_class_get (klass, 1);
	else
		aklass = mono_bounded_array_class_get (klass, rank, TRUE);

	return mono_type_get_object (mono_object_domain (type), &aklass->byval_arg);
}