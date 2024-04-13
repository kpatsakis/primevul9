static bool winbind_name_to_sid_string(struct pwb_context *ctx,
				       const char *user,
				       const char *name,
				       char *sid_list_buffer,
				       int sid_list_buffer_size)
{
	char sid_string[WBC_SID_STRING_BUFLEN];

	/* lookup name? */
	if (IS_SID_STRING(name)) {
		strlcpy(sid_string, name, sizeof(sid_string));
	} else {
		wbcErr wbc_status;
		struct wbcDomainSid sid;
		enum wbcSidType type;

		_pam_log_debug(ctx, LOG_DEBUG,
			       "no sid given, looking up: %s\n", name);

		wbc_status = wbcLookupName("", name, &sid, &type);
		if (!WBC_ERROR_IS_OK(wbc_status)) {
			_pam_log(ctx, LOG_INFO,
				 "could not lookup name: %s\n", name);
			return false;
		}

		wbcSidToStringBuf(&sid, sid_string, sizeof(sid_string));
	}

	if (!safe_append_string(sid_list_buffer, sid_string,
				sid_list_buffer_size)) {
		return false;
	}
	return true;
}