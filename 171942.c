bcon_error_t bson_append_bcon(bson *b, const bcon *bc) {
    return bson_append_bcon_with_state(b, bc, State_Element);
}