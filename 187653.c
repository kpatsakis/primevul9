EXPORTED int process_request(struct transaction_t *txn)
{
    int ret = 0;

    if (txn->req_tgt.namespace->premethod) {
        ret = txn->req_tgt.namespace->premethod(txn);
    }
    if (!ret) {
        const struct method_t *meth_t =
            &txn->req_tgt.namespace->methods[txn->meth];
        
        ret = (*meth_t->proc)(txn, meth_t->params);

        prometheus_increment(
            prometheus_lookup_label(http_methods[txn->meth].metric,
                                    txn->req_tgt.namespace->name));
    }

    if (ret == HTTP_UNAUTHORIZED) {
        /* User must authenticate */
        ret = client_need_auth(txn, 0);
    }

    return ret;
}