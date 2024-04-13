pdf14_buf_new(gs_int_rect *rect, bool has_tags, bool has_alpha_g,
              bool has_shape, bool idle, int n_chan, int num_spots,
              gs_memory_t *memory)
{

        /* Note that alpha_g is the alpha for the GROUP */
        /* This is distinct from the alpha that may also exist */
        /* for the objects within the group.  Hence it can introduce */
        /* yet another plane */

    pdf14_buf *result;
    pdf14_parent_color_t *new_parent_color;
    int rowstride = (rect->q.x - rect->p.x + 3) & -4;
    int height = (rect->q.y - rect->p.y);
    int n_planes = n_chan + (has_shape ? 1 : 0) + (has_alpha_g ? 1 : 0) +
                   (has_tags ? 1 : 0);
    int planestride;
    double dsize = (((double) rowstride) * height) * n_planes;

    if (dsize > (double)max_uint)
      return NULL;

    result = gs_alloc_struct(memory, pdf14_buf, &st_pdf14_buf,
                             "pdf14_buf_new");
    if (result == NULL)
        return result;

    result->backdrop = NULL;
    result->saved = NULL;
    result->isolated = false;
    result->knockout = false;
    result->has_alpha_g = has_alpha_g;
    result->has_shape = has_shape;
    result->has_tags = has_tags;
    result->rect = *rect;
    result->n_chan = n_chan;
    result->n_planes = n_planes;
    result->rowstride = rowstride;
    result->transfer_fn = NULL;
    result->matte_num_comps = 0;
    result->matte = NULL;
    result->mask_stack = NULL;
    result->idle = idle;
    result->mask_id = 0;
    result->num_spots = num_spots;
    new_parent_color = gs_alloc_struct(memory, pdf14_parent_color_t, &st_pdf14_clr,
                                                "pdf14_buf_new");
    if (new_parent_color == NULL) {
        gs_free_object(memory, result, "pdf14_buf_new");
        return NULL;
    }

    result->parent_color_info_procs = new_parent_color;
    result->parent_color_info_procs->get_cmap_procs = NULL;
    result->parent_color_info_procs->parent_color_mapping_procs = NULL;
    result->parent_color_info_procs->parent_color_comp_index = NULL;
    result->parent_color_info_procs->icc_profile = NULL;
    result->parent_color_info_procs->previous = NULL;
    result->parent_color_info_procs->encode = NULL;
    result->parent_color_info_procs->decode = NULL;
    if (height <= 0) {
        /* Empty clipping - will skip all drawings. */
        result->planestride = 0;
        result->data = 0;
    } else {
        planestride = rowstride * height;
        result->planestride = planestride;
        result->data = gs_alloc_bytes(memory, planestride * n_planes,
                                        "pdf14_buf_new");
        if (result->data == NULL) {
            gs_free_object(memory, result, "pdf14_buf_new");
            return NULL;
        }
        if (has_alpha_g) {
            int alpha_g_plane = n_chan + (has_shape ? 1 : 0);
            memset (result->data + alpha_g_plane * planestride, 0, planestride);
        }
        if (has_tags) {
            int tags_plane = n_chan + (has_shape ? 1 : 0) + (has_alpha_g ? 1 : 0);
            memset (result->data + tags_plane * planestride,
                    GS_UNTOUCHED_TAG, planestride);
        }
    }
    /* Initialize dirty box with an invalid rectangle (the reversed rectangle).
     * Any future drawing will make it valid again, so we won't blend back
     * more than we need. */
    result->dirty.p.x = rect->q.x;
    result->dirty.p.y = rect->q.y;
    result->dirty.q.x = rect->p.x;
    result->dirty.q.y = rect->p.y;
    return result;
}