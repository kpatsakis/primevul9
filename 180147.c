init_generic_context_from_args (MonoGenericContext *context, MonoArray *type_args, MonoArray *method_args)
{
	if (type_args)
		context->class_inst = mono_metadata_get_generic_inst (mono_array_length (type_args),
								      mono_array_addr (type_args, MonoType*, 0));
	else
		context->class_inst = NULL;
	if (method_args)
		context->method_inst = mono_metadata_get_generic_inst (mono_array_length (method_args),
								       mono_array_addr (method_args, MonoType*, 0));
	else
		context->method_inst = NULL;
}