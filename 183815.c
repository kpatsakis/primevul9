yang_free_import(struct ly_ctx *ctx, struct lys_import *imp, uint8_t start, uint8_t size)
{
    uint8_t i;

    for (i = start; i < size; ++i){
        free((char *)imp[i].module);
        lydict_remove(ctx, imp[i].prefix);
        lydict_remove(ctx, imp[i].dsc);
        lydict_remove(ctx, imp[i].ref);
        lys_extension_instances_free(ctx, imp[i].ext, imp[i].ext_size, NULL);
    }
}