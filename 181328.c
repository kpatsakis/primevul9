void v9fs_device_unrealize_common(V9fsState *s)
{
    if (s->ops && s->ops->cleanup) {
        s->ops->cleanup(&s->ctx);
    }
    if (s->ctx.fst) {
        fsdev_throttle_cleanup(s->ctx.fst);
    }
    g_free(s->tag);
    qp_table_destroy(&s->qpd_table);
    qp_table_destroy(&s->qpp_table);
    qp_table_destroy(&s->qpf_table);
    g_free(s->ctx.fs_root);
}