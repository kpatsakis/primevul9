static u8 dccp_feat_handle_nn_established(struct sock *sk, u8 mandatory, u8 opt,
					  u8 feat, u8 *val, u8 len)
{
	struct list_head *fn = &dccp_sk(sk)->dccps_featneg;
	const bool local = (opt == DCCPO_CONFIRM_R);
	struct dccp_feat_entry *entry;
	u8 type = dccp_feat_type(feat);
	dccp_feat_val fval;

	dccp_feat_print_opt(opt, feat, val, len, mandatory);

	/* Ignore non-mandatory unknown and non-NN features */
	if (type == FEAT_UNKNOWN) {
		if (local && !mandatory)
			return 0;
		goto fast_path_unknown;
	} else if (type != FEAT_NN) {
		return 0;
	}

	/*
	 * We don't accept empty Confirms, since in fast-path feature
	 * negotiation the values are enabled immediately after sending
	 * the Change option.
	 * Empty Changes on the other hand are invalid (RFC 4340, 6.1).
	 */
	if (len == 0 || len > sizeof(fval.nn))
		goto fast_path_unknown;

	if (opt == DCCPO_CHANGE_L) {
		fval.nn = dccp_decode_value_var(val, len);
		if (!dccp_feat_is_valid_nn_val(feat, fval.nn))
			goto fast_path_unknown;

		if (dccp_feat_push_confirm(fn, feat, local, &fval) ||
		    dccp_feat_activate(sk, feat, local, &fval))
			return DCCP_RESET_CODE_TOO_BUSY;

		/* set the `Ack Pending' flag to piggyback a Confirm */
		inet_csk_schedule_ack(sk);

	} else if (opt == DCCPO_CONFIRM_R) {
		entry = dccp_feat_list_lookup(fn, feat, local);
		if (entry == NULL || entry->state != FEAT_CHANGING)
			return 0;

		fval.nn = dccp_decode_value_var(val, len);
		/*
		 * Just ignore a value that doesn't match our current value.
		 * If the option changes twice within two RTTs, then at least
		 * one CONFIRM will be received for the old value after a
		 * new CHANGE was sent.
		 */
		if (fval.nn != entry->val.nn)
			return 0;

		/* Only activate after receiving the Confirm option (6.6.1). */
		dccp_feat_activate(sk, feat, local, &fval);

		/* It has been confirmed - so remove the entry */
		dccp_feat_list_pop(entry);

	} else {
		DCCP_WARN("Received illegal option %u\n", opt);
		goto fast_path_failed;
	}
	return 0;

fast_path_unknown:
	if (!mandatory)
		return dccp_push_empty_confirm(fn, feat, local);

fast_path_failed:
	return mandatory ? DCCP_RESET_CODE_MANDATORY_ERROR
			 : DCCP_RESET_CODE_OPTION_ERROR;
}