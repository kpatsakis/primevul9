check_tgs_flags(astgs_request_t r, KDC_REQ_BODY *b,
		krb5_const_principal tgt_name,
		const EncTicketPart *tgt, EncTicketPart *et)
{
    krb5_context context = r->context;
    KDCOptions f = b->kdc_options;

    if(f.validate){
	if (!tgt->flags.invalid || tgt->starttime == NULL) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Bad request to validate ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	if(*tgt->starttime > kdc_time){
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Early request to validate ticket");
	    return KRB5KRB_AP_ERR_TKT_NYV;
	}
	/* XXX  tkt = tgt */
	et->flags.invalid = 0;
    } else if (tgt->flags.invalid) {
	_kdc_audit_addreason((kdc_request_t)r,
                             "Ticket-granting ticket has INVALID flag set");
	return KRB5KRB_AP_ERR_TKT_INVALID;
    }

    if(f.forwardable){
	if (!tgt->flags.forwardable) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Bad request for forwardable ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	et->flags.forwardable = 1;
    }
    if(f.forwarded){
	if (!tgt->flags.forwardable) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Request to forward non-forwardable ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	et->flags.forwarded = 1;
	et->caddr = b->addresses;
    }
    if(tgt->flags.forwarded)
	et->flags.forwarded = 1;

    if(f.proxiable){
	if (!tgt->flags.proxiable) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Bad request for proxiable ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	et->flags.proxiable = 1;
    }
    if(f.proxy){
	if (!tgt->flags.proxiable) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Request to proxy non-proxiable ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	et->flags.proxy = 1;
	et->caddr = b->addresses;
    }
    if(tgt->flags.proxy)
	et->flags.proxy = 1;

    if(f.allow_postdate){
	if (!tgt->flags.may_postdate) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Bad request for post-datable ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	et->flags.may_postdate = 1;
    }
    if(f.postdated){
	if (!tgt->flags.may_postdate) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Bad request for postdated ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	if(b->from)
	    *et->starttime = *b->from;
	et->flags.postdated = 1;
	et->flags.invalid = 1;
    } else if (b->from && *b->from > kdc_time + context->max_skew) {
	_kdc_audit_addreason((kdc_request_t)r,
                             "Ticket cannot be postdated");
	return KRB5KDC_ERR_CANNOT_POSTDATE;
    }

    if(f.renewable){
	if (!tgt->flags.renewable || tgt->renew_till == NULL) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Bad request for renewable ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	et->flags.renewable = 1;
	ALLOC(et->renew_till);
	_kdc_fix_time(&b->rtime);
	*et->renew_till = *b->rtime;
    }
    if(f.renew){
	time_t old_life;
	if (!tgt->flags.renewable || tgt->renew_till == NULL) {
	    _kdc_audit_addreason((kdc_request_t)r,
                                 "Request to renew non-renewable ticket");
	    return KRB5KDC_ERR_BADOPTION;
	}
	old_life = tgt->endtime;
	if(tgt->starttime)
	    old_life -= *tgt->starttime;
	else
	    old_life -= tgt->authtime;
	et->endtime = *et->starttime + old_life;
	if (et->renew_till != NULL)
	    et->endtime = min(*et->renew_till, et->endtime);
    }

    /*
     * RFC 8062 section 3 defines an anonymous ticket as one containing
     * the anonymous principal and the anonymous ticket flag.
     */
    if (tgt->flags.anonymous &&
	!_kdc_is_anonymous(context, tgt_name)) {
	_kdc_audit_addreason((kdc_request_t)r,
                             "Anonymous ticket flag set without "
			 "anonymous principal");
	return KRB5KDC_ERR_BADOPTION;
    }

    /*
     * RFC 8062 section 4.2 states that if the TGT is anonymous, the
     * anonymous KDC option SHOULD be set, but it is not required.
     * Treat an anonymous TGT as if the anonymous flag was set.
     */
    if (is_anon_tgs_request_p(b, tgt))
	et->flags.anonymous = 1;

    return 0;
}