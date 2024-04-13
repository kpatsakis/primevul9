ves_icall_Type_GetPacking (MonoReflectionType *type, guint32 *packing, guint32 *size)
{
	MonoClass *klass = mono_class_from_mono_type (type->type);

	if (klass->image->dynamic) {
		MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder*)type;
		*packing = tb->packing_size;
		*size = tb->class_size;
	} else {
		mono_metadata_packing_from_typedef (klass->image, klass->type_token, packing, size);
	}
}