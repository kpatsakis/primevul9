EXPORTED void keepalive_response(struct transaction_t *txn)
{
    if (gotsigalrm) {
        response_header(HTTP_PROCESSING, txn);
    }
}