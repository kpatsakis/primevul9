ves_icall_get_property_info (MonoReflectionProperty *property, MonoPropertyInfo *info, PInfo req_info)
{
	MonoDomain *domain = mono_object_domain (property); 

	MONO_ARCH_SAVE_REGS;

	if ((req_info & PInfo_ReflectedType) != 0)
		MONO_STRUCT_SETREF (info, parent, mono_type_get_object (domain, &property->klass->byval_arg));
	else if ((req_info & PInfo_DeclaringType) != 0)
		MONO_STRUCT_SETREF (info, parent, mono_type_get_object (domain, &property->property->parent->byval_arg));

	if ((req_info & PInfo_Name) != 0)
		MONO_STRUCT_SETREF (info, name, mono_string_new (domain, property->property->name));

	if ((req_info & PInfo_Attributes) != 0)
		info->attrs = property->property->attrs;

	if ((req_info & PInfo_GetMethod) != 0)
		MONO_STRUCT_SETREF (info, get, property->property->get ?
							mono_method_get_object (domain, property->property->get, property->klass): NULL);
	
	if ((req_info & PInfo_SetMethod) != 0)
		MONO_STRUCT_SETREF (info, set, property->property->set ?
							mono_method_get_object (domain, property->property->set, property->klass): NULL);
	/* 
	 * There may be other methods defined for properties, though, it seems they are not exposed 
	 * in the reflection API 
	 */
}