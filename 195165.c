rpmRC rpmReadPackageFile(rpmts ts, FD_t fd, const char * fn, Header * hdrp)
{
    rpmRC rc;
    rpmVSFlags vsflags = rpmtsVSFlags(ts);
    rpmKeyring keyring = rpmtsGetKeyring(ts, 1);
    unsigned int keyid = 0;
    char *msg = NULL;

    if (fn == NULL)
	fn = Fdescr(fd);

    rc = rpmpkgRead(keyring, vsflags, fd, hdrp, &keyid, &msg);

    switch (rc) {
    case RPMRC_OK:		/* Signature is OK. */
	rpmlog(RPMLOG_DEBUG, "%s: %s\n", fn, msg);
	break;
    case RPMRC_NOTTRUSTED:	/* Signature is OK, but key is not trusted. */
    case RPMRC_NOKEY:		/* Public key is unavailable. */
	/* XXX Print NOKEY/NOTTRUSTED warning only once. */
    {	int lvl = (stashKeyid(keyid) ? RPMLOG_DEBUG : RPMLOG_WARNING);
	rpmlog(lvl, "%s: %s\n", fn, msg);
    }	break;
    case RPMRC_NOTFOUND:	/* Signature is unknown type or manifest. */
	/* msg == NULL is probably a manifest */
	if (msg)
	    rpmlog(RPMLOG_WARNING, "%s: %s\n", fn, msg);
	break;
    default:
    case RPMRC_FAIL:		/* Signature does not verify. */
	rpmlog(RPMLOG_ERR, "%s: %s\n", fn, msg);
	break;
    }
    rpmKeyringFree(keyring);
    free(msg);

    return rc;
}