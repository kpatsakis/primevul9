pdf14_buf_free(pdf14_buf *buf, gs_memory_t *memory)
{
    pdf14_parent_color_t *old_parent_color_info = buf->parent_color_info_procs;

    if (buf->mask_stack && buf->mask_stack->rc_mask)
        rc_decrement(buf->mask_stack->rc_mask, "pdf14_buf_free");

    gs_free_object(memory, buf->mask_stack, "pdf14_buf_free");
    gs_free_object(memory, buf->transfer_fn, "pdf14_buf_free");
    gs_free_object(memory, buf->matte, "pdf14_buf_free");
    gs_free_object(memory, buf->data, "pdf14_buf_free");

    while (old_parent_color_info) {
       if (old_parent_color_info->icc_profile != NULL) {
           rc_decrement(old_parent_color_info->icc_profile, "pdf14_buf_free");
       }
       buf->parent_color_info_procs = old_parent_color_info->previous;
       gs_free_object(memory, old_parent_color_info, "pdf14_buf_free");
       old_parent_color_info = buf->parent_color_info_procs;
    }

    gs_free_object(memory, buf->backdrop, "pdf14_buf_free");
    gs_free_object(memory, buf, "pdf14_buf_free");
}