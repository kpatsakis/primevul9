EXPORTED int http_allow_noauth_get(struct transaction_t *txn)
{
    /* Inverse logic: True means we *require* authentication */
    switch (txn->meth) {
    case METH_GET:
    case METH_HEAD:
        /* Let method processing function decide if auth is needed */
        return 0;
    default:
        return 1;
    }
}