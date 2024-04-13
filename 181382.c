static int qid_inode_prefix_hash_bits(V9fsPDU *pdu, dev_t dev)
{
    QpdEntry lookup = {
        .dev = dev
    }, *val;
    uint32_t hash = dev;
    VariLenAffix affix;

    val = qht_lookup(&pdu->s->qpd_table, &lookup, hash);
    if (!val) {
        val = g_malloc0(sizeof(QpdEntry));
        *val = lookup;
        affix = affixForIndex(pdu->s->qp_affix_next);
        val->prefix_bits = affix.bits;
        qht_insert(&pdu->s->qpd_table, val, hash, NULL);
        pdu->s->qp_ndevices++;
    }
    return val->prefix_bits;
}