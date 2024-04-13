static void coroutine_fn v9fs_version(void *opaque)
{
    ssize_t err;
    V9fsPDU *pdu = opaque;
    V9fsState *s = pdu->s;
    V9fsString version;
    size_t offset = 7;

    v9fs_string_init(&version);
    err = pdu_unmarshal(pdu, offset, "ds", &s->msize, &version);
    if (err < 0) {
        goto out;
    }
    trace_v9fs_version(pdu->tag, pdu->id, s->msize, version.data);

    virtfs_reset(pdu);

    if (!strcmp(version.data, "9P2000.u")) {
        s->proto_version = V9FS_PROTO_2000U;
    } else if (!strcmp(version.data, "9P2000.L")) {
        s->proto_version = V9FS_PROTO_2000L;
    } else {
        v9fs_string_sprintf(&version, "unknown");
        /* skip min. msize check, reporting invalid version has priority */
        goto marshal;
    }

    if (s->msize < P9_MIN_MSIZE) {
        err = -EMSGSIZE;
        error_report(
            "9pfs: Client requested msize < minimum msize ("
            stringify(P9_MIN_MSIZE) ") supported by this server."
        );
        goto out;
    }

    /* 8192 is the default msize of Linux clients */
    if (s->msize <= 8192 && !(s->ctx.export_flags & V9FS_NO_PERF_WARN)) {
        warn_report_once(
            "9p: degraded performance: a reasonable high msize should be "
            "chosen on client/guest side (chosen msize is <= 8192). See "
            "https://wiki.qemu.org/Documentation/9psetup#msize for details."
        );
    }

marshal:
    err = pdu_marshal(pdu, offset, "ds", s->msize, &version);
    if (err < 0) {
        goto out;
    }
    err += offset;
    trace_v9fs_version_return(pdu->tag, pdu->id, s->msize, version.data);
out:
    pdu_complete(pdu, err);
    v9fs_string_free(&version);
}