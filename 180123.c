type_from_name (const char *str, MonoBoolean ignoreCase)
{
	MonoType *type = NULL;
	MonoAssembly *assembly = NULL;
	MonoTypeNameParse info;
	char *temp_str = g_strdup (str);
	gboolean type_resolve = FALSE;

	MONO_ARCH_SAVE_REGS;

	/* mono_reflection_parse_type() mangles the string */
	if (!mono_reflection_parse_type (temp_str, &info)) {
		mono_reflection_free_type_info (&info);
		g_free (temp_str);
		return NULL;
	}

	if (info.assembly.name) {
		assembly = mono_assembly_load (&info.assembly, NULL, NULL);
	} else {
		MonoMethod *m = mono_method_get_last_managed ();
		MonoMethod *dest = m;

		mono_stack_walk_no_il (get_caller_no_reflection, &dest);
		if (!dest)
			dest = m;

		/*
		 * FIXME: mono_method_get_last_managed() sometimes returns NULL, thus
		 *        causing ves_icall_System_Reflection_Assembly_GetCallingAssembly()
		 *        to crash.  This only seems to happen in some strange remoting
		 *        scenarios and I was unable to figure out what's happening there.
		 *        Dec 10, 2005 - Martin.
		 */

		if (dest) {
			assembly = dest->klass->image->assembly;
			type_resolve = TRUE;
		} else {
			g_warning (G_STRLOC);
		}
	}

	if (assembly) {
		/* When loading from the current assembly, AppDomain.TypeResolve will not be called yet */
		type = mono_reflection_get_type (assembly->image, &info, ignoreCase, &type_resolve);
	}

	if (!info.assembly.name && !type) /* try mscorlib */
		type = mono_reflection_get_type (NULL, &info, ignoreCase, &type_resolve);

	if (assembly && !type && type_resolve) {
		type_resolve = FALSE; /* This will invoke TypeResolve if not done in the first 'if' */
		type = mono_reflection_get_type (assembly->image, &info, ignoreCase, &type_resolve);
	}

	mono_reflection_free_type_info (&info);
	g_free (temp_str);

	if (!type) 
		return NULL;

	return mono_type_get_object (mono_domain_get (), type);
}