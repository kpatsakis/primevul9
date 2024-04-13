static int v9fs_walk_marshal(V9fsPDU *pdu, uint16_t nwnames, V9fsQID *qids)
{
    int i;
    ssize_t err;
    size_t offset = 7;

    err = pdu_marshal(pdu, offset, "w", nwnames);
    if (err < 0) {
        return err;
    }
    offset += err;
    for (i = 0; i < nwnames; i++) {
        err = pdu_marshal(pdu, offset, "Q", &qids[i]);
        if (err < 0) {
            return err;
        }
        offset += err;
    }
    return offset;
}