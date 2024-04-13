c_pdf14trans_get_cropping(const gs_composite_t *pcte, int *ry, int *rheight,
                          int cropping_min, int cropping_max)
{
    gs_pdf14trans_t * pdf14pct = (gs_pdf14trans_t *) pcte;
    switch (pdf14pct->params.pdf14_op) {
        case PDF14_PUSH_DEVICE: return ALLBANDS; /* Applies to all bands. */
        case PDF14_POP_DEVICE:  return ALLBANDS; /* Applies to all bands. */
        case PDF14_ABORT_DEVICE: return ALLBANDS; /* Applies to all bands */
        case PDF14_BEGIN_TRANS_GROUP:
            {	gs_int_rect rect;

                pdf14_compute_group_device_int_rect(&pdf14pct->params.ctm,
                                                &pdf14pct->params.bbox, &rect);
                /* We have to crop this by the parent object.   */
                *ry = max(rect.p.y, cropping_min);
                *rheight = min(rect.q.y, cropping_max) - *ry;
                return PUSHCROP; /* Push cropping. */
            }
        case PDF14_BEGIN_TRANS_MASK:
            {	gs_int_rect rect;

                pdf14_compute_group_device_int_rect(&pdf14pct->params.ctm,
                                                    &pdf14pct->params.bbox, &rect);
                /* We have to crop this by the parent object and worry about the BC outside
                   the range, except for image SMask which don't affect areas outside the image */
                if ( pdf14pct->params.GrayBackground == 1.0 || pdf14pct->params.mask_is_image) {
                    /* In this case there will not be a background effect to
                       worry about.  The mask will not have any effect outside
                       the bounding box.  This is NOT the default or common case. */
                    *ry = max(rect.p.y, cropping_min);
                    *rheight = min(rect.q.y, cropping_max) - *ry;
                    return PUSHCROP; /* Push cropping. */
                }  else {
                    /* We need to make the soft mask range as large as the parent
                       due to the fact that the background color can have an impact
                       OUTSIDE the bounding box of the soft mask */
                    *ry = cropping_min;
                    *rheight = cropping_max - cropping_min;
                    if (pdf14pct->params.subtype == TRANSPARENCY_MASK_None)
                        return SAMEAS_PUSHCROP_BUTNOPUSH;
                    else
                        return PUSHCROP; /* Push cropping. */
                }
            }
        case PDF14_END_TRANS_GROUP: return POPCROP; /* Pop cropping. */
        case PDF14_END_TRANS_MASK: return POPCROP;   /* Pop the cropping */
        case PDF14_PUSH_TRANS_STATE: return CURRBANDS;
        case PDF14_POP_TRANS_STATE: return CURRBANDS;
        case PDF14_SET_BLEND_PARAMS: return ALLBANDS;
        case PDF14_PUSH_SMASK_COLOR: return POPCROP; /* Pop cropping. */
        case PDF14_POP_SMASK_COLOR: return POPCROP;   /* Pop the cropping */
    }
    return ALLBANDS;
}