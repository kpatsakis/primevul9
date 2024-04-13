yang_free_augment(struct ly_ctx *ctx, struct lys_node_augment *aug)
{
    lydict_remove(ctx, aug->target_name);
    lydict_remove(ctx, aug->dsc);
    lydict_remove(ctx, aug->ref);

    lys_iffeature_free(ctx, aug->iffeature, aug->iffeature_size, 0, NULL);
    lys_when_free(ctx, aug->when, NULL);
    yang_free_nodes(ctx, aug->child);
    lys_extension_instances_free(ctx, aug->ext, aug->ext_size, NULL);
}