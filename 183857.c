yang_free_uses(struct ly_ctx *ctx, struct lys_node_uses *uses)
{
    int i, j;

    for (i = 0; i < uses->refine_size; i++) {
        lydict_remove(ctx, uses->refine[i].target_name);
        lydict_remove(ctx, uses->refine[i].dsc);
        lydict_remove(ctx, uses->refine[i].ref);

        for (j = 0; j < uses->refine[i].must_size; j++) {
            lys_restr_free(ctx, &uses->refine[i].must[j], NULL);
        }
        free(uses->refine[i].must);

        for (j = 0; j < uses->refine[i].dflt_size; j++) {
            lydict_remove(ctx, uses->refine[i].dflt[j]);
        }
        free(uses->refine[i].dflt);

        if (uses->refine[i].target_type & LYS_CONTAINER) {
            lydict_remove(ctx, uses->refine[i].mod.presence);
        }
        lys_extension_instances_free(ctx, uses->refine[i].ext, uses->refine[i].ext_size, NULL);
    }
    free(uses->refine);

    lys_when_free(ctx, uses->when, NULL);
}