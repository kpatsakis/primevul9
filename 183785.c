yang_free_deviate(struct ly_ctx *ctx, struct lys_deviation *dev, uint index)
{
    uint i, j;

    for (i = index; i < dev->deviate_size; ++i) {
        lydict_remove(ctx, dev->deviate[i].units);

        if (dev->deviate[i].type) {
            yang_type_free(ctx, dev->deviate[i].type);
            free(dev->deviate[i].type);
        }

        for (j = 0; j < dev->deviate[i].dflt_size; ++j) {
            lydict_remove(ctx, dev->deviate[i].dflt[j]);
        }
        free(dev->deviate[i].dflt);

        for (j = 0; j < dev->deviate[i].must_size; ++j) {
            lys_restr_free(ctx, &dev->deviate[i].must[j], NULL);
        }
        free(dev->deviate[i].must);

        for (j = 0; j < dev->deviate[i].unique_size; ++j) {
            free(dev->deviate[i].unique[j].expr);
        }
        free(dev->deviate[i].unique);
        lys_extension_instances_free(ctx, dev->deviate[i].ext, dev->deviate[i].ext_size, NULL);
    }
}