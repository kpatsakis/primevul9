yang_free_include(struct ly_ctx *ctx, struct lys_include *inc, uint8_t start, uint8_t size)
{
    uint8_t i;

    for (i = start; i < size; ++i){
        free((char *)inc[i].submodule);
        lydict_remove(ctx, inc[i].dsc);
        lydict_remove(ctx, inc[i].ref);
        lys_extension_instances_free(ctx, inc[i].ext, inc[i].ext_size, NULL);
    }
}