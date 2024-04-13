static int unix_authenticate(void *instance, REQUEST *request)
{
#ifdef OSFSIA
	char		*info[2];
	char		*progname = "radius";
	SIAENTITY	*ent = NULL;

	info[0] = progname;
	info[1] = NULL;
	if (sia_ses_init (&ent, 1, info, NULL, name, NULL, 0, NULL) !=
	    SIASUCCESS)
		return RLM_MODULE_NOTFOUND;
	if ((ret = sia_ses_authent (NULL, passwd, ent)) != SIASUCCESS) {
		if (ret & SIASTOP)
			sia_ses_release (&ent);
		return RLM_MODULE_NOTFOUND;
	}
	if (sia_ses_estab (NULL, ent) != SIASUCCESS) {
		sia_ses_release (&ent);
		return RLM_MODULE_NOTFOUND;
	}
#else  /* OSFSIA */
	int rcode;
	VALUE_PAIR *vp = NULL;

	if (!request->password ||
	    (request->password->attribute != PW_USER_PASSWORD)) {
		radlog_request(L_AUTH, 0, request, "Attribute \"User-Password\" is required for authentication.");
		return RLM_MODULE_INVALID;
	}

	rcode = unix_getpw(instance, request, &vp);
	if (rcode != RLM_MODULE_UPDATED) return rcode;

	/*
	 *	0 means "ok"
	 */
	if (fr_crypt_check((char *) request->password->vp_strvalue,
			     (char *) vp->vp_strvalue) != 0) {
		radlog_request(L_AUTH, 0, request, "invalid password \"%s\"",
			       request->password->vp_strvalue);
		return RLM_MODULE_REJECT;
	}
#endif /* OSFFIA */

	return RLM_MODULE_OK;
}