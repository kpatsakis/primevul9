ves_icall_InternalInvoke (MonoReflectionMethod *method, MonoObject *this, MonoArray *params, MonoException **exc) 
{
	/* 
	 * Invoke from reflection is supposed to always be a virtual call (the API
	 * is stupid), mono_runtime_invoke_*() calls the provided method, allowing
	 * greater flexibility.
	 */
	MonoMethod *m = method->method;
	int pcount;
	void *obj = this;

	MONO_ARCH_SAVE_REGS;

	*exc = NULL;

	if (mono_security_get_mode () == MONO_SECURITY_MODE_CORE_CLR)
		mono_security_core_clr_ensure_reflection_access_method (m);

	if (!(m->flags & METHOD_ATTRIBUTE_STATIC)) {
		if (!mono_class_vtable_full (mono_object_domain (method), m->klass, FALSE)) {
			mono_gc_wbarrier_generic_store (exc, (MonoObject*) mono_class_get_exception_for_failure (m->klass));
			return NULL;
		}

		if (this) {
			if (!mono_object_isinst (this, m->klass)) {
				mono_gc_wbarrier_generic_store (exc, (MonoObject*) mono_exception_from_name_msg (mono_defaults.corlib, "System.Reflection", "TargetException", "Object does not match target type."));
				return NULL;
			}
			m = mono_object_get_virtual_method (this, m);
			/* must pass the pointer to the value for valuetype methods */
			if (m->klass->valuetype)
				obj = mono_object_unbox (this);
		} else if (strcmp (m->name, ".ctor") && !m->wrapper_type) {
			mono_gc_wbarrier_generic_store (exc, (MonoObject*) mono_exception_from_name_msg (mono_defaults.corlib, "System.Reflection", "TargetException", "Non-static method requires a target."));
			return NULL;
		}
	}

	pcount = params? mono_array_length (params): 0;
	if (pcount != mono_method_signature (m)->param_count) {
		mono_gc_wbarrier_generic_store (exc, (MonoObject*) mono_exception_from_name (mono_defaults.corlib, "System.Reflection", "TargetParameterCountException"));
		return NULL;
	}

	if ((m->klass->flags & TYPE_ATTRIBUTE_ABSTRACT) && !strcmp (m->name, ".ctor") && !this) {
		mono_gc_wbarrier_generic_store (exc, (MonoObject*) mono_exception_from_name_msg (mono_defaults.corlib, "System.Reflection", "TargetException", "Cannot invoke constructor of an abstract class."));
		return NULL;
	}

	if (m->klass->image->assembly->ref_only) {
		mono_gc_wbarrier_generic_store (exc, (MonoObject*) mono_get_exception_invalid_operation ("It is illegal to invoke a method on a type loaded using the ReflectionOnly api."));
		return NULL;
	}
	
	if (m->klass->rank && !strcmp (m->name, ".ctor")) {
		int i;
		mono_array_size_t *lengths;
		mono_array_size_t *lower_bounds;
		pcount = mono_array_length (params);
		lengths = alloca (sizeof (mono_array_size_t) * pcount);
		for (i = 0; i < pcount; ++i)
			lengths [i] = *(mono_array_size_t*) ((char*)mono_array_get (params, gpointer, i) + sizeof (MonoObject));

		if (m->klass->rank == pcount) {
			/* Only lengths provided. */
			lower_bounds = NULL;
		} else {
			g_assert (pcount == (m->klass->rank * 2));
			/* lower bounds are first. */
			lower_bounds = lengths;
			lengths += m->klass->rank;
		}

		return (MonoObject*)mono_array_new_full (mono_object_domain (params), m->klass, lengths, lower_bounds);
	}
	return mono_runtime_invoke_array (m, obj, params, NULL);
}