mono_class_is_constraint_compatible (MonoClass *candidate, MonoClass *target)
{
	if (candidate == target)
		return TRUE;
	if (target == mono_defaults.object_class)
			return TRUE;

	//setup_supertypes don't mono_class_init anything
	mono_class_setup_supertypes (candidate);
	mono_class_setup_supertypes (target);

	if (mono_class_has_parent (candidate, target))
		return TRUE;

	//if target is not a supertype it must be an interface
	if (!MONO_CLASS_IS_INTERFACE (target))
			return FALSE;

	if (candidate->image->dynamic && !candidate->wastypebuilder) {
		MonoReflectionTypeBuilder *tb = candidate->reflection_info;
		int j;
		if (tb->interfaces) {
			for (j = mono_array_length (tb->interfaces) - 1; j >= 0; --j) {
				MonoReflectionType *iface = mono_array_get (tb->interfaces, MonoReflectionType*, j);
				MonoClass *ifaceClass = mono_class_from_mono_type (iface->type);
				if (mono_class_is_constraint_compatible (ifaceClass, target)) {
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	return mono_class_interface_implements_interface (candidate, target);
}