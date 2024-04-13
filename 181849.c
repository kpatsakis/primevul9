lyp_ext_instance_rm(struct ly_ctx *ctx, struct lys_ext_instance ***ext, uint8_t *size, uint8_t index)
{
    uint8_t i;

    lys_extension_instances_free(ctx, (*ext)[index]->ext, (*ext)[index]->ext_size, NULL);
    lydict_remove(ctx, (*ext)[index]->arg_value);
    free((*ext)[index]);

    /* move the rest of the array */
    for (i = index + 1; i < (*size); i++) {
        (*ext)[i - 1] = (*ext)[i];
    }
    /* clean the last cell in the array structure */
    (*ext)[(*size) - 1] = NULL;
    /* the array is not reallocated here, just change its size */
    (*size) = (*size) - 1;

    if (!(*size)) {
        /* ext array is empty */
        free((*ext));
        ext = NULL;
    }
}