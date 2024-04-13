mono_lookup_internal_call (MonoMethod *method)
{
	char *sigstart;
	char *tmpsig;
	char mname [2048];
	int typelen = 0, mlen, siglen;
	gpointer res;
	const IcallTypeDesc *imap;

	g_assert (method != NULL);

	if (method->is_inflated)
		method = ((MonoMethodInflated *) method)->declaring;

	if (method->klass->nested_in) {
		int pos = concat_class_name (mname, sizeof (mname)-2, method->klass->nested_in);
		if (!pos)
			return NULL;

		mname [pos++] = '/';
		mname [pos] = 0;

		typelen = concat_class_name (mname+pos, sizeof (mname)-pos-1, method->klass);
		if (!typelen)
			return NULL;

		typelen += pos;
	} else {
		typelen = concat_class_name (mname, sizeof (mname), method->klass);
		if (!typelen)
			return NULL;
	}

	imap = find_class_icalls (mname);

	mname [typelen] = ':';
	mname [typelen + 1] = ':';

	mlen = strlen (method->name);
	memcpy (mname + typelen + 2, method->name, mlen);
	sigstart = mname + typelen + 2 + mlen;
	*sigstart = 0;

	tmpsig = mono_signature_get_desc (mono_method_signature (method), TRUE);
	siglen = strlen (tmpsig);
	if (typelen + mlen + siglen + 6 > sizeof (mname))
		return NULL;
	sigstart [0] = '(';
	memcpy (sigstart + 1, tmpsig, siglen);
	sigstart [siglen + 1] = ')';
	sigstart [siglen + 2] = 0;
	g_free (tmpsig);
	
	mono_loader_lock ();

	res = g_hash_table_lookup (icall_hash, mname);
	if (res) {
		mono_loader_unlock ();
		return res;
	}
	/* try without signature */
	*sigstart = 0;
	res = g_hash_table_lookup (icall_hash, mname);
	if (res) {
		mono_loader_unlock ();
		return res;
	}

	/* it wasn't found in the static call tables */
	if (!imap) {
		mono_loader_unlock ();
		return NULL;
	}
	res = find_method_icall (imap, sigstart - mlen);
	if (res) {
		mono_loader_unlock ();
		return res;
	}
	/* try _with_ signature */
	*sigstart = '(';
	res = find_method_icall (imap, sigstart - mlen);
	if (res) {
		mono_loader_unlock ();
		return res;
	}

	g_warning ("cant resolve internal call to \"%s\" (tested without signature also)", mname);
	g_print ("\nYour mono runtime and class libraries are out of sync.\n");
	g_print ("The out of sync library is: %s\n", method->klass->image->name);
	g_print ("\nWhen you update one from svn you need to update, compile and install\nthe other too.\n");
	g_print ("Do not report this as a bug unless you're sure you have updated correctly:\nyou probably have a broken mono install.\n");
	g_print ("If you see other errors or faults after this message they are probably related\n");
	g_print ("and you need to fix your mono install first.\n");

	mono_loader_unlock ();

	return NULL;
}