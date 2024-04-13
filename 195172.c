rpmRC rpmReadHeader(rpmts ts, FD_t fd, Header *hdrp, char ** msg)
{
    rpmRC rc;
    rpmKeyring keyring = rpmtsGetKeyring(ts, 1);
    rpmVSFlags vsflags = rpmtsVSFlags(ts);

    rc = rpmpkgReadHeader(keyring, vsflags, fd, hdrp, msg);

    rpmKeyringFree(keyring);
    return rc;
}