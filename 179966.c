ves_icall_System_Reflection_Assembly_InternalGetType (MonoReflectionAssembly *assembly, MonoReflectionModule *module, MonoString *name, MonoBoolean throwOnError, MonoBoolean ignoreCase)
{
	gchar *str;
	MonoType *type = NULL;
	MonoTypeNameParse info;
	gboolean type_resolve;

	MONO_ARCH_SAVE_REGS;

	/* On MS.NET, this does not fire a TypeResolve event */
	type_resolve = TRUE;
	str = mono_string_to_utf8 (name);
	/*g_print ("requested type %s in %s\n", str, assembly->assembly->aname.name);*/
	if (!mono_reflection_parse_type (str, &info)) {
		g_free (str);
		mono_reflection_free_type_info (&info);
		if (throwOnError) /* uhm: this is a parse error, though... */
			mono_raise_exception (mono_get_exception_type_load (name, NULL));
		/*g_print ("failed parse\n");*/
		return NULL;
	}

	if (info.assembly.name) {
		g_free (str);
		mono_reflection_free_type_info (&info);
		if (throwOnError) {
			/* 1.0 and 2.0 throw different exceptions */
			if (mono_defaults.generic_ilist_class)
				mono_raise_exception (mono_get_exception_argument (NULL, "Type names passed to Assembly.GetType() must not specify an assembly."));
			else
				mono_raise_exception (mono_get_exception_type_load (name, NULL));
		}
		return NULL;
	}

	if (module != NULL) {
		if (module->image)
			type = mono_reflection_get_type (module->image, &info, ignoreCase, &type_resolve);
		else
			type = NULL;
	}
	else
		if (assembly->assembly->dynamic) {
			/* Enumerate all modules */
			MonoReflectionAssemblyBuilder *abuilder = (MonoReflectionAssemblyBuilder*)assembly;
			int i;

			type = NULL;
			if (abuilder->modules) {
				for (i = 0; i < mono_array_length (abuilder->modules); ++i) {
					MonoReflectionModuleBuilder *mb = mono_array_get (abuilder->modules, MonoReflectionModuleBuilder*, i);
					type = mono_reflection_get_type (&mb->dynamic_image->image, &info, ignoreCase, &type_resolve);
					if (type)
						break;
				}
			}

			if (!type && abuilder->loaded_modules) {
				for (i = 0; i < mono_array_length (abuilder->loaded_modules); ++i) {
					MonoReflectionModule *mod = mono_array_get (abuilder->loaded_modules, MonoReflectionModule*, i);
					type = mono_reflection_get_type (mod->image, &info, ignoreCase, &type_resolve);
					if (type)
						break;
				}
			}
		}
		else
			type = mono_reflection_get_type (assembly->assembly->image, &info, ignoreCase, &type_resolve);
	g_free (str);
	mono_reflection_free_type_info (&info);
	if (!type) {
		MonoException *e = NULL;
		
		if (throwOnError)
			e = mono_get_exception_type_load (name, NULL);

		if (mono_loader_get_last_error () && mono_defaults.generic_ilist_class)
			e = mono_loader_error_prepare_exception (mono_loader_get_last_error ());

		mono_loader_clear_error ();

		if (e != NULL)
			mono_raise_exception (e);

		return NULL;
	}

	if (type->type == MONO_TYPE_CLASS) {
		MonoClass *klass = mono_type_get_class (type);

		if (mono_is_security_manager_active () && !klass->exception_type)
			/* Some security problems are detected during generic vtable construction */
			mono_class_setup_vtable (klass);
		/* need to report exceptions ? */
		if (throwOnError && klass->exception_type) {
			/* report SecurityException (or others) that occured when loading the assembly */
			MonoException *exc = mono_class_get_exception_for_failure (klass);
			mono_loader_clear_error ();
			mono_raise_exception (exc);
		} else if (klass->exception_type == MONO_EXCEPTION_SECURITY_INHERITANCEDEMAND) {
			return NULL;
		}
	}

	/* g_print ("got it\n"); */
	return mono_type_get_object (mono_object_domain (assembly), type);
}