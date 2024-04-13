ves_icall_MonoType_GetCorrespondingInflatedMethod (MonoReflectionType *type, 
                                                   MonoReflectionMethod* generic)
{
	MonoDomain *domain; 
	MonoClass *klass;
	MonoMethod *method;
	gpointer iter;
		
	MONO_ARCH_SAVE_REGS;

	domain = ((MonoObject *)type)->vtable->domain;

	klass = mono_class_from_mono_type (type->type);

	iter = NULL;
	while ((method = mono_class_get_methods (klass, &iter))) {
                if (method->token == generic->method->token)
                        return mono_method_get_object (domain, method, klass);
        }

        return NULL;
}