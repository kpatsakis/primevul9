is_anon_tgs_request_p(const KDC_REQ_BODY *b,
		      const EncTicketPart *tgt)
{
    KDCOptions f = b->kdc_options;

    /*
     * Versions of Heimdal from 1.0 to 7.6, inclusive, send both the
     * request-anonymous and cname-in-addl-tkt flags for constrained
     * delegation requests. A true anonymous TGS request will only
     * have the request-anonymous flag set. (A corollary of this is
     * that it is not possible to support anonymous constrained
     * delegation requests, although they would be of limited utility.)
     */
    return tgt->flags.anonymous ||
	(f.request_anonymous && !f.cname_in_addl_tkt && !b->additional_tickets);
}