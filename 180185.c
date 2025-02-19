mono_method_get_equivalent_method (MonoMethod *method, MonoClass *klass)
{
	int offset = -1, i;
	if (method->is_inflated && ((MonoMethodInflated*)method)->context.method_inst) {
		MonoMethodInflated *inflated = (MonoMethodInflated*)method;
		//method is inflated, we should inflate it on the other class
		MonoGenericContext ctx;
		ctx.method_inst = inflated->context.method_inst;
		ctx.class_inst = inflated->context.class_inst;
		if (klass->generic_class)
			ctx.class_inst = klass->generic_class->context.class_inst;
		else if (klass->generic_container)
			ctx.class_inst = klass->generic_container->context.class_inst;
		return mono_class_inflate_generic_method_full (inflated->declaring, klass, &ctx);
	}

	mono_class_setup_methods (method->klass);
	if (method->klass->exception_type)
		return NULL;
	for (i = 0; i < method->klass->method.count; ++i) {
		if (method->klass->methods [i] == method) {
			offset = i;
			break;
		}	
	}
	mono_class_setup_methods (klass);
	if (klass->exception_type)
		return NULL;
	g_assert (offset >= 0 && offset < klass->method.count);
	return klass->methods [offset];
}