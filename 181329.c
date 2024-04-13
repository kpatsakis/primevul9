static int qid_path_fullmap(V9fsPDU *pdu, const struct stat *stbuf,
                            uint64_t *path)
{
    QpfEntry lookup = {
        .dev = stbuf->st_dev,
        .ino = stbuf->st_ino
    }, *val;
    uint32_t hash = qpf_hash(lookup);
    VariLenAffix affix;

    val = qht_lookup(&pdu->s->qpf_table, &lookup, hash);

    if (!val) {
        if (pdu->s->qp_fullpath_next == 0) {
            /* no more files can be mapped :'( */
            error_report_once(
                "9p: No more prefixes available for remapping inodes from "
                "host to guest."
            );
            return -ENFILE;
        }

        val = g_malloc0(sizeof(QppEntry));
        *val = lookup;

        /* new unique inode and device combo */
        affix = affixForIndex(
            1ULL << (sizeof(pdu->s->qp_affix_next) * 8)
        );
        val->path = (pdu->s->qp_fullpath_next++ << affix.bits) | affix.value;
        pdu->s->qp_fullpath_next &= ((1ULL << (64 - affix.bits)) - 1);
        qht_insert(&pdu->s->qpf_table, val, hash, NULL);
    }

    *path = val->path;
    return 0;
}