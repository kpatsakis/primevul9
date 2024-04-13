custom_attrs_defined_internal (MonoObject *obj, MonoReflectionType *attr_type)
{
	MonoCustomAttrInfo *cinfo;
	gboolean found;

	cinfo = mono_reflection_get_custom_attrs_info (obj);
	if (!cinfo)
		return FALSE;
	found = mono_custom_attrs_has_attr (cinfo, mono_class_from_mono_type (attr_type->type));
	if (!cinfo->cached)
		mono_custom_attrs_free (cinfo);
	return found;
}