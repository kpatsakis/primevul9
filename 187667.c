EXPORTED void allow_hdr(struct transaction_t *txn,
                        const char *name, unsigned allow)
{
    const char *meths[] = {
        "OPTIONS, GET, HEAD", "POST", "PUT",
        "PATCH", "DELETE", "TRACE", "CONNECT", NULL
    };

    comma_list_hdr(txn, name, meths, allow);

    if (allow & ALLOW_DAV) {
        simple_hdr(txn, name, "PROPFIND, REPORT, COPY%s%s%s%s%s",
                   (allow & ALLOW_DELETE)    ? ", MOVE" : "",
                   (allow & ALLOW_PROPPATCH) ? ", PROPPATCH" : "",
                   (allow & ALLOW_MKCOL)     ? ", MKCOL" : "",
                   (allow & ALLOW_WRITE)     ? ", LOCK, UNLOCK" : "",
                   (allow & ALLOW_ACL)       ? ", ACL" : "");
        if ((allow & ALLOW_CAL) && (allow & ALLOW_MKCOL))
            simple_hdr(txn, name, "MKCALENDAR");
    }
}