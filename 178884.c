static u8 dccp_feat_confirm_recv(struct list_head *fn, u8 is_mandatory, u8 opt,
				 u8 feat, u8 *val, u8 len, const bool server)
{
	u8 *plist, plen, type = dccp_feat_type(feat);
	const bool local = (opt == DCCPO_CONFIRM_R);
	struct dccp_feat_entry *entry = dccp_feat_list_lookup(fn, feat, local);

	dccp_feat_print_opt(opt, feat, val, len, is_mandatory);

	if (entry == NULL) {	/* nothing queued: ignore or handle error */
		if (is_mandatory && type == FEAT_UNKNOWN)
			return DCCP_RESET_CODE_MANDATORY_ERROR;

		if (!local && type == FEAT_NN)		/* 6.3.2 */
			goto confirmation_failed;
		return 0;
	}

	if (entry->state != FEAT_CHANGING)		/* 6.6.2 */
		return 0;

	if (len == 0) {
		if (dccp_feat_must_be_understood(feat))	/* 6.6.7 */
			goto confirmation_failed;
		/*
		 * Empty Confirm during connection setup: this means reverting
		 * to the `old' value, which in this case is the default. Since
		 * we handle default values automatically when no other values
		 * have been set, we revert to the old value by removing this
		 * entry from the list.
		 */
		dccp_feat_list_pop(entry);
		return 0;
	}

	if (type == FEAT_NN) {
		if (len > sizeof(entry->val.nn))
			goto confirmation_failed;

		if (entry->val.nn == dccp_decode_value_var(val, len))
			goto confirmation_succeeded;

		DCCP_WARN("Bogus Confirm for non-existing value\n");
		goto confirmation_failed;
	}

	/*
	 * Parsing SP Confirms: the first element of @val is the preferred
	 * SP value which the peer confirms, the remainder depends on @len.
	 * Note that only the confirmed value need to be a valid SP value.
	 */
	if (!dccp_feat_is_valid_sp_val(feat, *val))
		goto confirmation_failed;

	if (len == 1) {		/* peer didn't supply a preference list */
		plist = val;
		plen  = len;
	} else {		/* preferred value + preference list */
		plist = val + 1;
		plen  = len - 1;
	}

	/* Check whether the peer got the reconciliation right (6.6.8) */
	if (dccp_feat_reconcile(&entry->val, plist, plen, server, 0) != *val) {
		DCCP_WARN("Confirm selected the wrong value %u\n", *val);
		return DCCP_RESET_CODE_OPTION_ERROR;
	}
	entry->val.sp.vec[0] = *val;

confirmation_succeeded:
	entry->state = FEAT_STABLE;
	return 0;

confirmation_failed:
	DCCP_WARN("Confirmation failed\n");
	return is_mandatory ? DCCP_RESET_CODE_MANDATORY_ERROR
			    : DCCP_RESET_CODE_OPTION_ERROR;
}