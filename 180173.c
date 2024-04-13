fill_reflection_assembly_name (MonoDomain *domain, MonoReflectionAssemblyName *aname, MonoAssemblyName *name, const char *absolute, gboolean by_default_version, gboolean default_publickey, gboolean default_token)
{
	static MonoMethod *create_culture = NULL;
	gpointer args [2];
	guint32 pkey_len;
	const char *pkey_ptr;
	gchar *codebase;
	MonoBoolean assembly_ref = 0;

	MONO_ARCH_SAVE_REGS;

	MONO_OBJECT_SETREF (aname, name, mono_string_new (domain, name->name));
	aname->major = name->major;
	aname->minor = name->minor;
	aname->build = name->build;
	aname->flags = name->flags;
	aname->revision = name->revision;
	aname->hashalg = name->hash_alg;
	aname->versioncompat = 1; /* SameMachine (default) */

	if (by_default_version)
		MONO_OBJECT_SETREF (aname, version, create_version (domain, name->major, name->minor, name->build, name->revision));

	codebase = NULL;
	if (absolute != NULL && *absolute != '\0') {
		const gchar *prepend = "file://";
		gchar *result;

		codebase = g_strdup (absolute);

#if PLATFORM_WIN32
		{
			gint i;
			for (i = strlen (codebase) - 1; i >= 0; i--)
				if (codebase [i] == '\\')
					codebase [i] = '/';

			if (*codebase == '/' && *(codebase + 1) == '/') {
				prepend = "file:";
			} else {
				prepend = "file:///";
			}
		}
#endif
		result = g_strconcat (prepend, codebase, NULL);
		g_free (codebase);
		codebase = result;
	}

	if (codebase) {
		MONO_OBJECT_SETREF (aname, codebase, mono_string_new (domain, codebase));
		g_free (codebase);
	}

	if (!create_culture) {
		MonoMethodDesc *desc = mono_method_desc_new ("System.Globalization.CultureInfo:CreateCulture(string,bool)", TRUE);
		create_culture = mono_method_desc_search_in_image (desc, mono_defaults.corlib);
		g_assert (create_culture);
		mono_method_desc_free (desc);
	}

	if (name->culture) {
		args [0] = mono_string_new (domain, name->culture);
		args [1] = &assembly_ref;
		MONO_OBJECT_SETREF (aname, cultureInfo, mono_runtime_invoke (create_culture, NULL, args, NULL));
	}

	if (name->public_key) {
		pkey_ptr = (char*)name->public_key;
		pkey_len = mono_metadata_decode_blob_size (pkey_ptr, &pkey_ptr);

		MONO_OBJECT_SETREF (aname, publicKey, mono_array_new (domain, mono_defaults.byte_class, pkey_len));
		memcpy (mono_array_addr (aname->publicKey, guint8, 0), pkey_ptr, pkey_len);
		aname->flags |= ASSEMBLYREF_FULL_PUBLIC_KEY_FLAG;
	} else if (default_publickey) {
		MONO_OBJECT_SETREF (aname, publicKey, mono_array_new (domain, mono_defaults.byte_class, 0));
		aname->flags |= ASSEMBLYREF_FULL_PUBLIC_KEY_FLAG;
	}

	/* MonoAssemblyName keeps the public key token as an hexadecimal string */
	if (name->public_key_token [0]) {
		int i, j;
		char *p;

		MONO_OBJECT_SETREF (aname, keyToken, mono_array_new (domain, mono_defaults.byte_class, 8));
		p = mono_array_addr (aname->keyToken, char, 0);

		for (i = 0, j = 0; i < 8; i++) {
			*p = g_ascii_xdigit_value (name->public_key_token [j++]) << 4;
			*p |= g_ascii_xdigit_value (name->public_key_token [j++]);
			p++;
		}
	} else if (default_token) {
		MONO_OBJECT_SETREF (aname, keyToken, mono_array_new (domain, mono_defaults.byte_class, 0));
	}
}