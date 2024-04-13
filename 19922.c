gopherReadReply(const Comm::ConnectionPointer &conn, char *buf, size_t len, Comm::Flag flag, int xerrno, void *data)
{
    GopherStateData *gopherState = (GopherStateData *)data;
    StoreEntry *entry = gopherState->entry;
    int clen;
    int bin;
    size_t read_sz = BUFSIZ;
#if USE_DELAY_POOLS
    DelayId delayId = entry->mem_obj->mostBytesAllowed();
#endif

    /* Bail out early on Comm::ERR_CLOSING - close handlers will tidy up for us */

    if (flag == Comm::ERR_CLOSING) {
        return;
    }

    assert(buf == gopherState->replybuf);

    // XXX: Should update delayId, statCounter, etc. before bailing
    if (!entry->isAccepting()) {
        debugs(10, 3, "terminating due to bad " << *entry);
        // TODO: Do not abuse connection for triggering cleanup.
        gopherState->serverConn->close();
        return;
    }

#if USE_DELAY_POOLS
    read_sz = delayId.bytesWanted(1, read_sz);
#endif

    /* leave one space for \0 in gopherToHTML */

    if (flag == Comm::OK && len > 0) {
#if USE_DELAY_POOLS
        delayId.bytesIn(len);
#endif

        statCounter.server.all.kbytes_in += len;
        statCounter.server.other.kbytes_in += len;
    }

    debugs(10, 5, HERE << conn << " read len=" << len);

    if (flag == Comm::OK && len > 0) {
        AsyncCall::Pointer nil;
        commSetConnTimeout(conn, Config.Timeout.read, nil);
        ++IOStats.Gopher.reads;

        for (clen = len - 1, bin = 0; clen; ++bin)
            clen >>= 1;

        ++IOStats.Gopher.read_hist[bin];

        HttpRequest *req = gopherState->fwd->request;
        if (req->hier.bodyBytesRead < 0) {
            req->hier.bodyBytesRead = 0;
            // first bytes read, update Reply flags:
            gopherState->reply_->sources |= HttpMsg::srcGopher;
        }

        req->hier.bodyBytesRead += len;
    }

    if (flag != Comm::OK) {
        debugs(50, DBG_IMPORTANT, MYNAME << "error reading: " << xstrerr(xerrno));

        if (ignoreErrno(xerrno)) {
            AsyncCall::Pointer call = commCbCall(5,4, "gopherReadReply",
                                                 CommIoCbPtrFun(gopherReadReply, gopherState));
            comm_read(conn, buf, read_sz, call);
        } else {
            ErrorState *err = new ErrorState(ERR_READ_ERROR, Http::scInternalServerError, gopherState->fwd->request);
            err->xerrno = xerrno;
            gopherState->fwd->fail(err);
            gopherState->serverConn->close();
        }
    } else if (len == 0 && entry->isEmpty()) {
        gopherState->fwd->fail(new ErrorState(ERR_ZERO_SIZE_OBJECT, Http::scServiceUnavailable, gopherState->fwd->request));
        gopherState->serverConn->close();
    } else if (len == 0) {
        /* Connection closed; retrieval done. */
        /* flush the rest of data in temp buf if there is one. */

        if (gopherState->conversion != GopherStateData::NORMAL)
            gopherEndHTML(gopherState);

        entry->timestampsSet();
        entry->flush();
        gopherState->fwd->complete();
        gopherState->serverConn->close();
    } else {
        if (gopherState->conversion != GopherStateData::NORMAL) {
            gopherToHTML(gopherState, buf, len);
        } else {
            entry->append(buf, len);
        }
        AsyncCall::Pointer call = commCbCall(5,4, "gopherReadReply",
                                             CommIoCbPtrFun(gopherReadReply, gopherState));
        comm_read(conn, buf, read_sz, call);
    }
}