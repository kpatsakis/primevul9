static bool winbind_name_to_sid_string(struct pwb_context *ctx,
				       const char *user,
				       const char *name,
				       char *sid_list_buffer,
				       int sid_list_buffer_size)
{
	const char* sid_string = NULL;
	char *sid_str = NULL;

	/* lookup name? */
	if (IS_SID_STRING(name)) {
		sid_string = name;
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

		wbc_status = wbcSidToString(&sid, &sid_str);
		if (!WBC_ERROR_IS_OK(wbc_status)) {
			return false;
		}

		sid_string = sid_str;
	}

	if (!safe_append_string(sid_list_buffer, sid_string,
				sid_list_buffer_size)) {
		wbcFreeMemory(sid_str);
		return false;
	}

	wbcFreeMemory(sid_str);
	return true;
}