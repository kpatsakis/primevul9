static int bcon_error(bson *b, const bcon *bc, size_t i, bcon_error_t err) {
    b->err = err;
    b->errstr = bcon_errstr[err];
    return BCON_ERROR;
}