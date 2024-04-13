rpmRC headerCheck(rpmts ts, const void * uh, size_t uc, char ** msg)
{
    rpmRC rc;
    rpmVSFlags vsflags = rpmtsVSFlags(ts);
    rpmKeyring keyring = rpmtsGetKeyring(ts, 1);

    rpmswEnter(rpmtsOp(ts, RPMTS_OP_DIGEST), 0);
    rc = headerVerify(keyring, vsflags, uh, uc, msg);
    rpmswExit(rpmtsOp(ts, RPMTS_OP_DIGEST), uc);
    rpmKeyringFree(keyring);

    return rc;
}