static void qpf_table_init(struct qht *ht)
{
    qht_init(ht, qpf_cmp_func, 1 << 16, QHT_MODE_AUTO_RESIZE);
}