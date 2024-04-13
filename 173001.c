c_pdf14trans_write(const gs_composite_t	* pct, byte * data, uint * psize,
                   gx_device_clist_writer *cdev)
{
    const gs_pdf14trans_params_t * pparams = &((const gs_pdf14trans_t *)pct)->params;
    int need, avail = *psize;
    byte buf[MAX_CLIST_TRANSPARENCY_BUFFER_SIZE]; /* Must be large enough
        to fit the data written below. We don't implement a dynamic check for
        the buffer owerflow, assuming that the consistency is verified in the
        coding phase. See the definition of MAX_CLIST_TRANSPARENCY_BUFFER_SIZE. */
    byte * pbuf = buf;
    int opcode = pparams->pdf14_op;
    int mask_size = 0;
    uint mask_id = 0;
    int code;
    bool found_icc;
    int64_t hashcode = 0;
    cmm_profile_t *icc_profile;
    gsicc_rendering_param_t render_cond;
    cmm_dev_profile_t *dev_profile;
    /* We maintain and update working copies until we actually write the clist */
    int pdf14_needed = cdev->pdf14_needed;
    int trans_group_level = cdev->pdf14_trans_group_level;
    int smask_level = cdev->pdf14_smask_level;

    code = dev_proc((gx_device *) cdev, get_profile)((gx_device *) cdev,
                                                     &dev_profile);
    if (code < 0)
        return code;
    gsicc_extract_profile(GS_UNKNOWN_TAG, dev_profile, &icc_profile,
                          &render_cond);
    *pbuf++ = opcode;			/* 1 byte */
    switch (opcode) {
        default:			/* Should not occur. */
            break;
        case PDF14_PUSH_DEVICE:
            trans_group_level = 0;
            cdev->pdf14_smask_level = 0;
            cdev->page_pdf14_needed = false;
            put_value(pbuf, pparams->num_spot_colors);
            put_value(pbuf, pparams->is_pattern);
            /* If we happen to be going to a color space like CIELAB then
               we are going to do our blending in default RGB and convert
               to CIELAB at the end.  To do this, we need to store the
               default RGB profile in the clist so that we can grab it
               later on during the clist read back and put image command */
            if (icc_profile->data_cs == gsCIELAB || icc_profile->islab) {
                /* Get the default RGB profile.  Set the device hash code
                   so that we can extract it during the put_image operation. */
                cdev->trans_dev_icc_hash = pparams->iccprofile->hashcode;
                found_icc =
                    clist_icc_searchtable(cdev, pparams->iccprofile->hashcode);
                if (!found_icc) {
                    /* Add it to the table */
                    clist_icc_addentry(cdev, pparams->iccprofile->hashcode,
                                       pparams->iccprofile);
                }
            }
            break;
        case PDF14_POP_DEVICE:
            pdf14_needed = false;		/* reset pdf14_needed */
            trans_group_level = 0;
            smask_level = 0;
            put_value(pbuf, pparams->is_pattern);
            break;
        case PDF14_END_TRANS_GROUP:
            trans_group_level--;	/* if now at page level, pdf14_needed will be updated */
            if (smask_level == 0 && trans_group_level == 0)
                pdf14_needed = cdev->page_pdf14_needed;
            break;			/* No data */
        case PDF14_BEGIN_TRANS_GROUP:
            pdf14_needed = true;		/* the compositor will be needed while reading */
            trans_group_level++;
            code = c_pdf14trans_write_ctm(&pbuf, pparams);
            if (code < 0)
                return code;
            *pbuf++ = (pparams->Isolated & 1) + ((pparams->Knockout & 1) << 1);
            *pbuf++ = pparams->blend_mode;
            *pbuf++ = pparams->group_color;
            put_value(pbuf, pparams->group_color_numcomps);
            put_value(pbuf, pparams->opacity.alpha);
            put_value(pbuf, pparams->shape.alpha);
            put_value(pbuf, pparams->bbox);
            mask_id = pparams->mask_id;
            put_value(pbuf, mask_id);
            /* Color space information maybe ICC based
               in this case we need to store the ICC
               profile or the ID if it is cached already */
            if (pparams->group_color == ICC) {
                /* Check if it is already in the ICC clist table */
                hashcode = pparams->iccprofile->hashcode;
                found_icc = clist_icc_searchtable(cdev, hashcode);
                if (!found_icc) {
                    /* Add it to the table */
                    clist_icc_addentry(cdev, hashcode, pparams->iccprofile);
                    put_value(pbuf, hashcode);
                } else {
                    /* It will be in the clist. Just write out the hashcode */
                    put_value(pbuf, hashcode);
                }
            } else {
                put_value(pbuf, hashcode);
            }
            break;
        case PDF14_BEGIN_TRANS_MASK:
            if (pparams->subtype != TRANSPARENCY_MASK_None) {
                pdf14_needed = true;		/* the compositor will be needed while reading */
                smask_level++;
            }
            code = c_pdf14trans_write_ctm(&pbuf, pparams);
            if (code < 0)
                return code;
            put_value(pbuf, pparams->subtype);
            *pbuf++ = pparams->group_color;
            put_value(pbuf, pparams->group_color_numcomps);
            *pbuf++ = pparams->replacing;
            *pbuf++ = pparams->function_is_identity;
            *pbuf++ = pparams->Background_components;
            *pbuf++ = pparams->Matte_components;
            put_value(pbuf, pparams->bbox);
            mask_id = pparams->mask_id;
            put_value(pbuf, mask_id);
            if (pparams->Background_components) {
                const int l = sizeof(pparams->Background[0]) * pparams->Background_components;

                memcpy(pbuf, pparams->Background, l);
                pbuf += l;
                memcpy(pbuf, &pparams->GrayBackground, sizeof(pparams->GrayBackground));
                pbuf += sizeof(pparams->GrayBackground);
            }
            if (pparams->Matte_components) {
                const int m = sizeof(pparams->Matte[0]) * pparams->Matte_components;

                memcpy(pbuf, pparams->Matte, m);
                pbuf += m;
            }
            if (!pparams->function_is_identity)
                mask_size = sizeof(pparams->transfer_fn);
            /* Color space information may be ICC based
               in this case we need to store the ICC
               profile or the ID if it is cached already */
            if (pparams->group_color == ICC) {
                /* Check if it is already in the ICC clist table */
                hashcode = pparams->iccprofile->hashcode;
                found_icc = clist_icc_searchtable(cdev, hashcode);
                if (!found_icc) {
                    /* Add it to the table */
                    clist_icc_addentry(cdev, hashcode, pparams->iccprofile);
                    put_value(pbuf, hashcode);
                } else {
                    /* It will be in the clist. Just write out the hashcode */
                    put_value(pbuf, hashcode);
                }
            } else {
                put_value(pbuf, hashcode);
            }
            break;
        case PDF14_END_TRANS_MASK:
            smask_level--;
            if (smask_level == 0 && trans_group_level == 0)
                pdf14_needed = cdev->page_pdf14_needed;
            break;
        case PDF14_SET_BLEND_PARAMS:
            if (pparams->blend_mode != BLEND_MODE_Normal || pparams->opacity.alpha != 1.0 ||
                pparams->shape.alpha != 1.0)
                pdf14_needed = true;		/* the compositor will be needed while reading */
            else if (smask_level == 0 && trans_group_level == 0)
                pdf14_needed = false;		/* At page level, set back to false */
            if (smask_level == 0 && trans_group_level == 0)
                cdev->page_pdf14_needed = pdf14_needed;         /* save for after popping to page level */
            *pbuf++ = pparams->changed;
            if (pparams->changed & PDF14_SET_BLEND_MODE)
                *pbuf++ = pparams->blend_mode;
            if (pparams->changed & PDF14_SET_TEXT_KNOCKOUT)
                *pbuf++ = pparams->text_knockout;
            if (pparams->changed & PDF14_SET_OPACITY_ALPHA)
                put_value(pbuf, pparams->opacity.alpha);
            if (pparams->changed & PDF14_SET_SHAPE_ALPHA)
                put_value(pbuf, pparams->shape.alpha);
            if (pparams->changed & PDF14_SET_OVERPRINT)
                put_value(pbuf, pparams->overprint);
            if (pparams->changed & PDF14_SET_OVERPRINT_MODE)
                put_value(pbuf, pparams->overprint_mode);
            break;
        case PDF14_PUSH_TRANS_STATE:
            break;
        case PDF14_POP_TRANS_STATE:
            break;
        case PDF14_PUSH_SMASK_COLOR:
            return 0;   /* We really should never be here */
            break;
        case PDF14_POP_SMASK_COLOR:
            return 0;   /* We really should never be here */
            break;
    }

    /* check for fit */
    need = (pbuf - buf) + mask_size;
    *psize = need;
    if (need > avail) {
        if (avail)
            return_error(gs_error_rangecheck);
        else
            return gs_error_rangecheck;
    }

    /* If we are writing more than the maximum ever expected,
     * return a rangecheck error. Second check is for Coverity
     */
    if ((need + 3 > MAX_CLIST_COMPOSITOR_SIZE) ||
        (need + 3 - mask_size > MAX_CLIST_TRANSPARENCY_BUFFER_SIZE) )
        return_error(gs_error_rangecheck);

    /* Copy our serialized data into the output buffer */
    memcpy(data, buf, need - mask_size);
    if (mask_size)	/* Include the transfer mask data if present */
        memcpy(data + need - mask_size, pparams->transfer_fn, mask_size);
    if_debug3m('v', cdev->memory,
               "[v] c_pdf14trans_write: opcode = %s mask_id=%d need = %d\n",
               pdf14_opcode_names[opcode], mask_id, need);
    cdev->pdf14_needed = pdf14_needed;          /* all OK to update */
    cdev->pdf14_trans_group_level = trans_group_level;
    cdev->pdf14_smask_level = smask_level;
    return 0;
}