ves_icall_InternalExecute (MonoReflectionMethod *method, MonoObject *this, MonoArray *params, MonoArray **outArgs) 
{
	MonoDomain *domain = mono_object_domain (method); 
	MonoMethod *m = method->method;
	MonoMethodSignature *sig = mono_method_signature (m);
	MonoArray *out_args;
	MonoObject *result;
	int i, j, outarg_count = 0;

	MONO_ARCH_SAVE_REGS;

	if (m->klass == mono_defaults.object_class) {

		if (!strcmp (m->name, "FieldGetter")) {
			MonoClass *k = this->vtable->klass;
			MonoString *name;
			char *str;
			
			/* If this is a proxy, then it must be a CBO */
			if (k == mono_defaults.transparent_proxy_class) {
				MonoTransparentProxy *tp = (MonoTransparentProxy*) this;
				this = tp->rp->unwrapped_server;
				g_assert (this);
				k = this->vtable->klass;
			}
			
			name = mono_array_get (params, MonoString *, 1);
			str = mono_string_to_utf8 (name);
		
			do {
				MonoClassField* field = mono_class_get_field_from_name (k, str);
				if (field) {
					MonoClass *field_klass =  mono_class_from_mono_type (field->type);
					if (field_klass->valuetype)
						result = mono_value_box (domain, field_klass, (char *)this + field->offset);
					else 
						result = *((gpointer *)((char *)this + field->offset));
				
					out_args = mono_array_new (domain, mono_defaults.object_class, 1);
					mono_gc_wbarrier_generic_store (outArgs, (MonoObject*) out_args);
					mono_array_setref (out_args, 0, result);
					g_free (str);
					return NULL;
				}
				k = k->parent;
			} while (k);

			g_free (str);
			g_assert_not_reached ();

		} else if (!strcmp (m->name, "FieldSetter")) {
			MonoClass *k = this->vtable->klass;
			MonoString *name;
			guint32 size;
			gint32 align;
			char *str;
			
			/* If this is a proxy, then it must be a CBO */
			if (k == mono_defaults.transparent_proxy_class) {
				MonoTransparentProxy *tp = (MonoTransparentProxy*) this;
				this = tp->rp->unwrapped_server;
				g_assert (this);
				k = this->vtable->klass;
			}
			
			name = mono_array_get (params, MonoString *, 1);
			str = mono_string_to_utf8 (name);
		
			do {
				MonoClassField* field = mono_class_get_field_from_name (k, str);
				if (field) {
					MonoClass *field_klass =  mono_class_from_mono_type (field->type);
					MonoObject *val = mono_array_get (params, gpointer, 2);

					if (field_klass->valuetype) {
						size = mono_type_size (field->type, &align);
#ifdef HAVE_SGEN_GC
						mono_gc_wbarrier_value_copy ((char *)this + field->offset, (char*)val + sizeof (MonoObject), 1, field_klass);
#endif
						memcpy ((char *)this + field->offset, 
							((char *)val) + sizeof (MonoObject), size);
					} else {
						mono_gc_wbarrier_set_field (this, (char*)this + field->offset, val);
					}
				
					out_args = mono_array_new (domain, mono_defaults.object_class, 0);
					mono_gc_wbarrier_generic_store (outArgs, (MonoObject*) out_args);

					g_free (str);
					return NULL;
				}
				
				k = k->parent;
			} while (k);

			g_free (str);
			g_assert_not_reached ();

		}
	}

	for (i = 0; i < mono_array_length (params); i++) {
		if (sig->params [i]->byref) 
			outarg_count++;
	}

	out_args = mono_array_new (domain, mono_defaults.object_class, outarg_count);
	
	/* handle constructors only for objects already allocated */
	if (!strcmp (method->method->name, ".ctor"))
		g_assert (this);

	/* This can be called only on MBR objects, so no need to unbox for valuetypes. */
	g_assert (!method->method->klass->valuetype);
	result = mono_runtime_invoke_array (method->method, this, params, NULL);

	for (i = 0, j = 0; i < mono_array_length (params); i++) {
		if (sig->params [i]->byref) {
			gpointer arg;
			arg = mono_array_get (params, gpointer, i);
			mono_array_setref (out_args, j, arg);
			j++;
		}
	}

	mono_gc_wbarrier_generic_store (outArgs, (MonoObject*) out_args);

	return result;
}