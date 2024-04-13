free_yang_common(struct lys_module *module, struct lys_node *node)
{
    uint i;
    yang_tpdf_free(module->ctx, module->tpdf, 0, module->tpdf_size);
    module->tpdf_size = 0;
    yang_free_ident_base(module->ident, 0, module->ident_size);
    yang_free_nodes(module->ctx, node);
    for (i = 0; i < module->augment_size; ++i) {
        yang_free_augment(module->ctx, &module->augment[i]);
    }
    module->augment_size = 0;
    for (i = 0; i < module->deviation_size; ++i) {
        yang_free_deviate(module->ctx, &module->deviation[i], 0);
        free(module->deviation[i].deviate);
        lydict_remove(module->ctx, module->deviation[i].target_name);
        lydict_remove(module->ctx, module->deviation[i].dsc);
        lydict_remove(module->ctx, module->deviation[i].ref);
    }
    module->deviation_size = 0;
}