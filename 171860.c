MONGO_EXPORT int bson_append_code_w_scope( bson *b, const char *name, const char *code, const bson *scope ) {
    return bson_append_code_w_scope_n( b, name, code, strlen ( code ), scope );
}