static rpmRC rpmpkgRead(rpmKeyring keyring, rpmVSFlags vsflags, 
			FD_t fd,
			Header * hdrp, unsigned int *keyidp, char **msg)
{
    pgpDigParams sig = NULL;
    Header sigh = NULL;
    rpmTagVal sigtag;
    struct rpmtd_s sigtd;
    struct sigtInfo_s sinfo;
    Header h = NULL;
    rpmRC rc = RPMRC_FAIL;	/* assume failure */
    int leadtype = -1;
    headerGetFlags hgeflags = HEADERGET_DEFAULT;

    if (hdrp) *hdrp = NULL;

    rpmtdReset(&sigtd);

    if ((rc = rpmLeadRead(fd, NULL, &leadtype, msg)) != RPMRC_OK) {
	/* Avoid message spew on manifests */
	if (rc == RPMRC_NOTFOUND) {
	    *msg = _free(*msg);
	}
	goto exit;
    }

    /* Read the signature header. */
    rc = rpmReadSignature(fd, &sigh, RPMSIGTYPE_HEADERSIG, msg);

    if (rc != RPMRC_OK) {
	goto exit;
    }

#define	_chk(_mask, _tag) \
	(sigtag == 0 && !(vsflags & (_mask)) && headerIsEntry(sigh, (_tag)))

    /*
     * Figger the most effective means of verification available, prefer
     * signatures over digests. Legacy header+payload entries are not used.
     * DSA will be preferred over RSA if both exist because tested first.
     */
    sigtag = 0;
    if (_chk(RPMVSF_NODSAHEADER, RPMSIGTAG_DSA)) {
	sigtag = RPMSIGTAG_DSA;
    } else if (_chk(RPMVSF_NORSAHEADER, RPMSIGTAG_RSA)) {
	sigtag = RPMSIGTAG_RSA;
    } else if (_chk(RPMVSF_NOSHA1HEADER, RPMSIGTAG_SHA1)) {
	sigtag = RPMSIGTAG_SHA1;
    }

    /* Read the metadata, computing digest(s) on the fly. */
    h = NULL;

    rc = rpmpkgReadHeader(keyring, vsflags, fd, &h, msg);

    if (rc != RPMRC_OK || h == NULL) {
	goto exit;
    }

    /* Any digests or signatures to check? */
    if (sigtag == 0) {
	rc = RPMRC_OK;
	goto exit;
    }

    /* Free up any previous "ok" message before signature/digest check */
    *msg = _free(*msg);

    /* Retrieve the tag parameters from the signature header. */
    if (!headerGet(sigh, sigtag, &sigtd, hgeflags)) {
	rc = RPMRC_FAIL;
	goto exit;
    }

    if (rpmSigInfoParse(&sigtd, "package", &sinfo, &sig, msg) == RPMRC_OK) {
	struct rpmtd_s utd;
	DIGEST_CTX ctx = rpmDigestInit(sinfo.hashalgo, RPMDIGEST_NONE);

	if (headerGet(h, RPMTAG_HEADERIMMUTABLE, &utd, hgeflags)) {
	    rpmDigestUpdate(ctx, rpm_header_magic, sizeof(rpm_header_magic));
	    rpmDigestUpdate(ctx, utd.data, utd.count);
	    rpmtdFreeData(&utd);
	}
	/** @todo Implement disable/enable/warn/error/anal policy. */
	rc = rpmVerifySignature(keyring, &sigtd, sig, ctx, msg);

	rpmDigestFinal(ctx, NULL, NULL, 0);
    } else {
	rc = RPMRC_FAIL;
    }

exit:
    if (rc != RPMRC_FAIL && h != NULL && hdrp != NULL) {
	/* Retrofit RPMTAG_SOURCEPACKAGE to srpms for compatibility */
	if (leadtype == RPMLEAD_SOURCE && headerIsSource(h)) {
	    if (!headerIsEntry(h, RPMTAG_SOURCEPACKAGE)) {
		uint32_t one = 1;
		headerPutUint32(h, RPMTAG_SOURCEPACKAGE, &one, 1);
	    }
	}
	/*
 	 * Try to make sure binary rpms have RPMTAG_SOURCERPM set as that's
 	 * what we use for differentiating binary vs source elsewhere.
 	 */
	if (!headerIsEntry(h, RPMTAG_SOURCEPACKAGE) && headerIsSource(h)) {
	    headerPutString(h, RPMTAG_SOURCERPM, "(none)");
	}
	/* 
         * Convert legacy headers on the fly. Not having immutable region
         * equals a truly ancient package, do full retrofit. OTOH newer
         * packages might have been built with --nodirtokens, test and handle
         * the non-compressed filelist case separately.
         */
	if (!headerIsEntry(h, RPMTAG_HEADERIMMUTABLE))
	    headerConvert(h, HEADERCONV_RETROFIT_V3);
	else if (headerIsEntry(h, RPMTAG_OLDFILENAMES))
	    headerConvert(h, HEADERCONV_COMPRESSFILELIST);
	
	/* Append (and remap) signature tags to the metadata. */
	headerMergeLegacySigs(h, sigh);

	/* Bump reference count for return. */
	*hdrp = headerLink(h);

	if (keyidp)
	    *keyidp = getKeyid(sig);
    }
    rpmtdFreeData(&sigtd);
    h = headerFree(h);
    pgpDigParamsFree(sig);
    sigh = rpmFreeSignature(sigh);
    return rc;
}