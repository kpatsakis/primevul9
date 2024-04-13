static void qpp_table_init(struct qht *ht)
{
    qht_init(ht, qpp_cmp_func, 1, QHT_MODE_AUTO_RESIZE);
}