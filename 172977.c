pdf14_discard_trans_layer(gx_device *dev, gs_gstate * pgs)
{
    pdf14_device *pdev = (pdf14_device *)dev;
    /* The things that need to be cleaned up */
    pdf14_ctx *ctx = pdev->ctx;
    pdf14_smaskcolor_t *smaskcolor = pdev->smaskcolor;
    pdf14_parent_color_t *group_procs = pdev->trans_group_parent_cmap_procs;

    /* Free up the smask color */
    if (smaskcolor != NULL) {
        smaskcolor->ref_count = 1;
        pdf14_decrement_smask_color(pgs, dev);
        pdev->smaskcolor = NULL;
    }

    /* Free up the nested color procs and decrement the profiles */
    if (group_procs != NULL) {
        while (group_procs->previous != NULL)
            pdf14_pop_parent_color(dev, pgs);
        gs_free_object(dev->memory, group_procs, "pdf14_discard_trans_layer");
        pdev->trans_group_parent_cmap_procs = NULL;
    }

    /* Start the contex clean up */
    if (ctx != NULL) {
        pdf14_buf *buf, *next;
        pdf14_parent_color_t *procs, *prev_procs;

        if (ctx->mask_stack != NULL) {
            pdf14_free_mask_stack(ctx, ctx->memory);
        }

        /* Now the stack of buffers */
        for (buf = ctx->stack; buf != NULL; buf = next) {
            next = buf->saved;

            gs_free_object(ctx->memory, buf->transfer_fn, "pdf14_discard_trans_layer");
            gs_free_object(ctx->memory, buf->matte, "pdf14_discard_trans_layer");
            gs_free_object(ctx->memory, buf->data, "pdf14_discard_trans_layer");
            gs_free_object(ctx->memory, buf->backdrop, "pdf14_discard_trans_layer");
            /* During the soft mask push, the mask_stack was copied (not moved) from
               the ctx to the tos mask_stack. We are done with this now so it is safe
               to free this one object */
            gs_free_object(ctx->memory, buf->mask_stack, "pdf14_discard_trans_layer");
            for (procs = buf->parent_color_info_procs; procs != NULL; procs = prev_procs) {
                prev_procs = procs->previous;
                gs_free_object(ctx->memory, procs, "pdf14_discard_trans_layer");
            }
            gs_free_object(ctx->memory, buf, "pdf14_discard_trans_layer");
        }
        /* Finally the context itself */
        gs_free_object (ctx->memory, ctx, "pdf14_discard_trans_layer");
        pdev->ctx = NULL;
    }
    return 0;
}