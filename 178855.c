static u8 dccp_feat_change_recv(struct list_head *fn, u8 is_mandatory, u8 opt,
				u8 feat, u8 *val, u8 len, const bool server)
{
	u8 defval, type = dccp_feat_type(feat);
	const bool local = (opt == DCCPO_CHANGE_R);
	struct dccp_feat_entry *entry;
	dccp_feat_val fval;

	if (len == 0 || type == FEAT_UNKNOWN)		/* 6.1 and 6.6.8 */
		goto unknown_feature_or_value;

	dccp_feat_print_opt(opt, feat, val, len, is_mandatory);

	/*
	 *	Negotiation of NN features: Change R is invalid, so there is no
	 *	simultaneous negotiation; hence we do not look up in the list.
	 */
	if (type == FEAT_NN) {
		if (local || len > sizeof(fval.nn))
			goto unknown_feature_or_value;

		/* 6.3.2: "The feature remote MUST accept any valid value..." */
		fval.nn = dccp_decode_value_var(val, len);
		if (!dccp_feat_is_valid_nn_val(feat, fval.nn))
			goto unknown_feature_or_value;

		return dccp_feat_push_confirm(fn, feat, local, &fval);
	}

	/*
	 *	Unidirectional/simultaneous negotiation of SP features (6.3.1)
	 */
	entry = dccp_feat_list_lookup(fn, feat, local);
	if (entry == NULL) {
		/*
		 * No particular preferences have been registered. We deal with
		 * this situation by assuming that all valid values are equally
		 * acceptable, and apply the following checks:
		 * - if the peer's list is a singleton, we accept a valid value;
		 * - if we are the server, we first try to see if the peer (the
		 *   client) advertises the default value. If yes, we use it,
		 *   otherwise we accept the preferred value;
		 * - else if we are the client, we use the first list element.
		 */
		if (dccp_feat_clone_sp_val(&fval, val, 1))
			return DCCP_RESET_CODE_TOO_BUSY;

		if (len > 1 && server) {
			defval = dccp_feat_default_value(feat);
			if (dccp_feat_preflist_match(&defval, 1, val, len) > -1)
				fval.sp.vec[0] = defval;
		} else if (!dccp_feat_is_valid_sp_val(feat, fval.sp.vec[0])) {
			kfree(fval.sp.vec);
			goto unknown_feature_or_value;
		}

		/* Treat unsupported CCIDs like invalid values */
		if (feat == DCCPF_CCID && !ccid_support_check(fval.sp.vec, 1)) {
			kfree(fval.sp.vec);
			goto not_valid_or_not_known;
		}

		return dccp_feat_push_confirm(fn, feat, local, &fval);

	} else if (entry->state == FEAT_UNSTABLE) {	/* 6.6.2 */
		return 0;
	}

	if (dccp_feat_reconcile(&entry->val, val, len, server, true)) {
		entry->empty_confirm = false;
	} else if (is_mandatory) {
		return DCCP_RESET_CODE_MANDATORY_ERROR;
	} else if (entry->state == FEAT_INITIALISING) {
		/*
		 * Failed simultaneous negotiation (server only): try to `save'
		 * the connection by checking whether entry contains the default
		 * value for @feat. If yes, send an empty Confirm to signal that
		 * the received Change was not understood - which implies using
		 * the default value.
		 * If this also fails, we use Reset as the last resort.
		 */
		WARN_ON(!server);
		defval = dccp_feat_default_value(feat);
		if (!dccp_feat_reconcile(&entry->val, &defval, 1, server, true))
			return DCCP_RESET_CODE_OPTION_ERROR;
		entry->empty_confirm = true;
	}
	entry->needs_confirm   = true;
	entry->needs_mandatory = false;
	entry->state	       = FEAT_STABLE;
	return 0;

unknown_feature_or_value:
	if (!is_mandatory)
		return dccp_push_empty_confirm(fn, feat, local);

not_valid_or_not_known:
	return is_mandatory ? DCCP_RESET_CODE_MANDATORY_ERROR
			    : DCCP_RESET_CODE_OPTION_ERROR;
}