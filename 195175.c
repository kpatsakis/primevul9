static rpmRC headerSigVerify(rpmKeyring keyring, rpmVSFlags vsflags,
			     int il, int dl, int ril, int rdl,
			     entryInfo pe, unsigned char * dataStart,
			     char **buf)
{
    size_t siglen = 0;
    rpmRC rc = RPMRC_FAIL;
    pgpDigParams sig = NULL;
    struct rpmtd_s sigtd;
    struct entryInfo_s info, einfo;
    struct sigtInfo_s sinfo;

    rpmtdReset(&sigtd);
    memset(&info, 0, sizeof(info));
    memset(&einfo, 0, sizeof(einfo));

    /* Find a header-only digest/signature tag. */
    for (int i = ril; i < il; i++) {
	if (headerVerifyInfo(1, dl, pe+i, &einfo, 0) != -1) {
	    rasprintf(buf,
		_("tag[%d]: BAD, tag %d type %d offset %d count %d"),
		i, einfo.tag, einfo.type,
		einfo.offset, einfo.count);
	    goto exit;
	}

	switch (einfo.tag) {
	case RPMTAG_SHA1HEADER: {
	    size_t blen = 0;
	    unsigned const char * b;
	    if (vsflags & RPMVSF_NOSHA1HEADER)
		break;
	    for (b = dataStart + einfo.offset; *b != '\0'; b++) {
		if (strchr("0123456789abcdefABCDEF", *b) == NULL)
		    break;
		blen++;
	    }
	    if (einfo.type != RPM_STRING_TYPE || *b != '\0' || blen != 40)
	    {
		rasprintf(buf, _("hdr SHA1: BAD, not hex"));
		goto exit;
	    }
	    if (info.tag == 0) {
		info = einfo;	/* structure assignment */
		siglen = blen + 1;
	    }
	    } break;
	case RPMTAG_RSAHEADER:
	    if (vsflags & RPMVSF_NORSAHEADER)
		break;
	    if (einfo.type != RPM_BIN_TYPE) {
		rasprintf(buf, _("hdr RSA: BAD, not binary"));
		goto exit;
	    }
	    info = einfo;	/* structure assignment */
	    siglen = info.count;
	    break;
	case RPMTAG_DSAHEADER:
	    if (vsflags & RPMVSF_NODSAHEADER)
		break;
	    if (einfo.type != RPM_BIN_TYPE) {
		rasprintf(buf, _("hdr DSA: BAD, not binary"));
		goto exit;
	    }
	    info = einfo;	/* structure assignment */
	    siglen = info.count;
	    break;
	default:
	    break;
	}
    }

    /* No header-only digest/signature found, get outta here */
    if (info.tag == 0) {
	rc = RPMRC_NOTFOUND;
	goto exit;
    }

    sigtd.tag = info.tag;
    sigtd.type = info.type;
    sigtd.count = info.count;
    sigtd.data = memcpy(xmalloc(siglen), dataStart + info.offset, siglen);
    sigtd.flags = RPMTD_ALLOCED;

    if (rpmSigInfoParse(&sigtd, "header", &sinfo, &sig, buf))
	goto exit;

    if (sinfo.hashalgo) {
	DIGEST_CTX ctx = rpmDigestInit(sinfo.hashalgo, RPMDIGEST_NONE);
	int32_t ildl[2] = { htonl(ril), htonl(rdl) };

	rpmDigestUpdate(ctx, rpm_header_magic, sizeof(rpm_header_magic));
	rpmDigestUpdate(ctx, ildl, sizeof(ildl));
	rpmDigestUpdate(ctx, pe, (ril * sizeof(*pe)));
	rpmDigestUpdate(ctx, dataStart, rdl);

	rc = rpmVerifySignature(keyring, &sigtd, sig, ctx, buf);

    	rpmDigestFinal(ctx, NULL, NULL, 0);
    }

exit:
    rpmtdFreeData(&sigtd);
    pgpDigParamsFree(sig);

    return rc;
}