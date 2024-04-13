ves_icall_System_Reflection_Module_ResolveStringToken (MonoImage *image, guint32 token, MonoResolveTokenError *error)
{
	int index = mono_metadata_token_index (token);

	*error = ResolveTokenError_Other;

	/* Validate token */
	if (mono_metadata_token_code (token) != MONO_TOKEN_STRING) {
		*error = ResolveTokenError_BadTable;
		return NULL;
	}

	if (image->dynamic)
		return mono_lookup_dynamic_token_class (image, token, FALSE, NULL, NULL);

	if ((index <= 0) || (index >= image->heap_us.size)) {
		*error = ResolveTokenError_OutOfRange;
		return NULL;
	}

	/* FIXME: What to do if the index points into the middle of a string ? */

	return mono_ldstr (mono_domain_get (), image, index);
}