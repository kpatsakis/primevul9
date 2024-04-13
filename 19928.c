gopherSendComplete(const Comm::ConnectionPointer &conn, char *, size_t size, Comm::Flag errflag, int xerrno, void *data)
{
    GopherStateData *gopherState = (GopherStateData *) data;
    StoreEntry *entry = gopherState->entry;
    debugs(10, 5, HERE << conn << " size: " << size << " errflag: " << errflag);

    if (size > 0) {
        fd_bytes(conn->fd, size, FD_WRITE);
        statCounter.server.all.kbytes_out += size;
        statCounter.server.other.kbytes_out += size;
    }

    if (!entry->isAccepting()) {
        debugs(10, 3, "terminating due to bad " << *entry);
        // TODO: Do not abuse connection for triggering cleanup.
        gopherState->serverConn->close();
        return;
    }

    if (errflag) {
        ErrorState *err;
        err = new ErrorState(ERR_WRITE_ERROR, Http::scServiceUnavailable, gopherState->fwd->request);
        err->xerrno = xerrno;
        err->port = gopherState->fwd->request->url.port();
        err->url = xstrdup(entry->url());
        gopherState->fwd->fail(err);
        gopherState->serverConn->close();
        return;
    }

    /*
     * OK. We successfully reach remote site.  Start MIME typing
     * stuff.  Do it anyway even though request is not HTML type.
     */
    entry->buffer();

    gopherMimeCreate(gopherState);

    switch (gopherState->type_id) {

    case GOPHER_DIRECTORY:
        /* we got to convert it first */
        gopherState->conversion = GopherStateData::HTML_DIR;
        gopherState->HTML_header_added = 0;
        break;

    case GOPHER_INDEX:
        /* we got to convert it first */
        gopherState->conversion = GopherStateData::HTML_INDEX_RESULT;
        gopherState->HTML_header_added = 0;
        break;

    case GOPHER_CSO:
        /* we got to convert it first */
        gopherState->conversion = GopherStateData::HTML_CSO_RESULT;
        gopherState->cso_recno = 0;
        gopherState->HTML_header_added = 0;
        break;

    default:
        gopherState->conversion = GopherStateData::NORMAL;
        entry->flush();
    }

    /* Schedule read reply. */
    AsyncCall::Pointer call =  commCbCall(5,5, "gopherReadReply",
                                          CommIoCbPtrFun(gopherReadReply, gopherState));
    entry->delayAwareRead(conn, gopherState->replybuf, BUFSIZ, call);
}