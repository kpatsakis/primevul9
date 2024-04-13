    GopherStateData(FwdState *aFwd) :
        entry(aFwd->entry),
        conversion(NORMAL),
        HTML_header_added(0),
        HTML_pre(0),
        type_id(GOPHER_FILE /* '0' */),
        cso_recno(0),
        len(0),
        buf(NULL),
        fwd(aFwd)
    {
        *request = 0;
        buf = (char *)memAllocate(MEM_4K_BUF);
        entry->lock("gopherState");
        *replybuf = 0;
    }