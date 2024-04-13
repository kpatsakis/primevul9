static rpmRC rpmpkgReadHeader(rpmKeyring keyring, rpmVSFlags vsflags, 
		       FD_t fd, Header *hdrp, char ** msg)
{
    char *buf = NULL;
    int32_t block[4];
    int32_t il;
    int32_t dl;
    int32_t * ei = NULL;
    size_t uc;
    size_t nb;
    Header h = NULL;
    rpmRC rc = RPMRC_FAIL;		/* assume failure */
    int xx;

    if (hdrp)
	*hdrp = NULL;
    if (msg)
	*msg = NULL;

    memset(block, 0, sizeof(block));
    if ((xx = Freadall(fd, block, sizeof(block))) != sizeof(block)) {
	rasprintf(&buf, 
		_("hdr size(%d): BAD, read returned %d"), (int)sizeof(block), xx);
	goto exit;
    }
    if (memcmp(block, rpm_header_magic, sizeof(rpm_header_magic))) {
	rasprintf(&buf, _("hdr magic: BAD"));
	goto exit;
    }
    il = ntohl(block[2]);
    if (hdrchkTags(il)) {
	rasprintf(&buf, _("hdr tags: BAD, no. of tags(%d) out of range"), il);
	goto exit;
    }
    dl = ntohl(block[3]);
    if (hdrchkData(dl)) {
	rasprintf(&buf,
		  _("hdr data: BAD, no. of bytes(%d) out of range"), dl);
	goto exit;
    }

    nb = (il * sizeof(struct entryInfo_s)) + dl;
    uc = sizeof(il) + sizeof(dl) + nb;
    ei = xmalloc(uc);
    ei[0] = block[2];
    ei[1] = block[3];
    if ((xx = Freadall(fd, (char *)&ei[2], nb)) != nb) {
	rasprintf(&buf, _("hdr blob(%zd): BAD, read returned %d"), nb, xx);
	goto exit;
    }

    /* Sanity check header tags */
    rc = headerVerify(keyring, vsflags, ei, uc, &buf);
    if (rc != RPMRC_OK)
	goto exit;

    /* OK, blob looks sane, load the header. */
    h = headerImport(ei, uc, 0);
    if (h == NULL) {
	free(buf);
	rasprintf(&buf, _("hdr load: BAD"));
	rc = RPMRC_FAIL;
        goto exit;
    }
    ei = NULL;	/* XXX will be freed with header */
    
exit:
    if (hdrp && h && rc == RPMRC_OK)
	*hdrp = headerLink(h);
    free(ei);
    headerFree(h);

    if (msg != NULL && *msg == NULL && buf != NULL) {
	*msg = buf;
    } else {
	free(buf);
    }

    return rc;
}