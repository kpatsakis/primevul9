pdf14_begin_typed_image(gx_device * dev, const gs_gstate * pgs,
                           const gs_matrix *pmat, const gs_image_common_t *pic,
                           const gs_int_rect * prect,
                           const gx_drawing_color * pdcolor,
                           const gx_clip_path * pcpath, gs_memory_t * mem,
                           gx_image_enum_common_t ** pinfo)
{
    const gs_image_t *pim = (const gs_image_t *)pic;
    int code;

    /* If we are filling an image mask with a pattern that has a transparency
       then we need to do some special handling */
    if (pim->ImageMask) {
        if (pdcolor != NULL && gx_dc_is_pattern1_color(pdcolor)) {
            if( gx_pattern1_get_transptr(pdcolor) != NULL){
                /* If we are in a final run through here for this case then
                   go ahead and push the transparency group.   Also, update
                   the proc for the pattern color so that we used the
                   appropriate fill operation.  Note that the group
                   is popped and the proc will be reset when we flush the
                   image data.  This is handled in a special pdf14 image
                   renderer which will end up installed for this case.
                   Detect setting of begin_image to gx_no_begin_image.
                   (final recursive call) */
                if (dev->procs.begin_image != gx_default_begin_image) {
                    code = pdf14_patt_trans_image_fill(dev, pgs, pmat, pic,
                                                prect, pdcolor, pcpath, mem,
                                                pinfo);
                    return code;
                }
            }
        }
    }
    pdf14_set_marking_params(dev, pgs);
    return gx_default_begin_typed_image(dev, pgs, pmat, pic, prect, pdcolor,
                                        pcpath, mem, pinfo);
}