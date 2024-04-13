static char* winbind_upn_to_username(struct pwb_context *ctx,
				     const char *upn)
{
	char sep;
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;
	struct wbcDomainSid sid;
	enum wbcSidType type;
	char *domain = NULL;
	char *name;
	char *p;

	/* This cannot work when the winbind separator = @ */

	sep = winbind_get_separator(ctx);
	if (!sep || sep == '@') {
		return NULL;
	}

	name = talloc_strdup(ctx, upn);
	if (!name) {
		return NULL;
	}
	if ((p = strchr(name, '@')) != NULL) {
		*p = 0;
		domain = p + 1;
	}

	/* Convert the UPN to a SID */

	wbc_status = wbcLookupName(domain, name, &sid, &type);
	if (!WBC_ERROR_IS_OK(wbc_status)) {
		return NULL;
	}

	/* Convert the the SID back to the sAMAccountName */

	wbc_status = wbcLookupSid(&sid, &domain, &name, &type);
	if (!WBC_ERROR_IS_OK(wbc_status)) {
		return NULL;
	}

	return talloc_asprintf(ctx, "%s%c%s", domain, sep, name);
}