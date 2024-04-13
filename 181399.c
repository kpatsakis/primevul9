static void qp_table_destroy(struct qht *ht)
{
    if (!ht || !ht->map) {
        return;
    }
    qht_iter(ht, qp_table_remove, NULL);
    qht_destroy(ht);
}