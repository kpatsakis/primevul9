wtf_push_ucs(Str os, wc_uint32 ucs, wc_status *st)
{
    wc_ccs ccs;

    if (ucs >= WC_C_LANGUAGE_TAG0 && ucs <= WC_C_CANCEL_TAG) {
	if (! WcOption.use_language_tag)
	    return;
	if (ucs == WC_C_LANGUAGE_TAG)
	    st->tag = Strnew_size(4);
	else if (ucs == WC_C_CANCEL_TAG) {
	    st->tag = NULL;
	    st->ntag = 0;
	}  else if (st->tag && ucs >= WC_C_TAG_SPACE)
	    Strcat_char(st->tag, (char)(ucs & 0x7f));
	return;
    }
    if (st->tag) {
	st->ntag = wc_ucs_put_tag(st->tag->ptr);
	st->tag = NULL;
    }
    if (ucs < 0x80) {
	if (st->ntag)
	    wtf_push(os, WC_CCS_UCS_TAG,  wc_ucs_to_ucs_tag(ucs, st->ntag));
	else
	    Strcat_char(os, (char)ucs);
    } else {
	ccs = wc_ucs_to_ccs(ucs);
	if (st->ntag && ucs <= WC_C_UNICODE_END) {
	    ccs = wc_ccs_ucs_to_ccs_ucs_tag(ccs);
	    ucs = wc_ucs_to_ucs_tag(ucs, st->ntag);
	}
	wtf_push(os, ccs, ucs);
    }
}