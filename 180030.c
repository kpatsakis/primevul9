property_info_get_type_modifiers (MonoReflectionProperty *property, MonoBoolean optional)
{
	MonoType *type = get_property_type (property->property);
	MonoImage *image = property->klass->image;

	if (!type)
		return NULL;
	return type_array_from_modifiers (image, type, optional);
}