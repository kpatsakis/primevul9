static int qid_path_suffixmap(V9fsPDU *pdu, const struct stat *stbuf,
                              uint64_t *path)
{
    const int ino_hash_bits = qid_inode_prefix_hash_bits(pdu, stbuf->st_dev);
    QppEntry lookup = {
        .dev = stbuf->st_dev,
        .ino_prefix = (uint16_t) (stbuf->st_ino >> (64 - ino_hash_bits))
    }, *val;
    uint32_t hash = qpp_hash(lookup);

    val = qht_lookup(&pdu->s->qpp_table, &lookup, hash);

    if (!val) {
        if (pdu->s->qp_affix_next == 0) {
            /* we ran out of affixes */
            warn_report_once(
                "9p: Potential degraded performance of inode remapping"
            );
            return -ENFILE;
        }

        val = g_malloc0(sizeof(QppEntry));
        *val = lookup;

        /* new unique inode affix and device combo */
        val->qp_affix_index = pdu->s->qp_affix_next++;
        val->qp_affix = affixForIndex(val->qp_affix_index);
        qht_insert(&pdu->s->qpp_table, val, hash, NULL);
    }
    /* assuming generated affix to be suffix type, not prefix */
    *path = (stbuf->st_ino << val->qp_affix.bits) | val->qp_affix.value;
    return 0;
}