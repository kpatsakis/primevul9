bcon_error_t bson_from_bcon(bson *b, const bcon *bc) {
    bcon_error_t ret = BSON_OK;
    bson_init( b );
    ret = bson_append_bcon_with_state( b, bc, State_Element );
    if (ret != BCON_OK) return ret;
    ret = bson_finish( b );
    return ( ret == BSON_OK ? BCON_OK : BCON_BSON_ERROR );
}