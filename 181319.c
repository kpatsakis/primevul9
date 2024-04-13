static void qpd_table_init(struct qht *ht)
{
    qht_init(ht, qpd_cmp_func, 1, QHT_MODE_AUTO_RESIZE);
}