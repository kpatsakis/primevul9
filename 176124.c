OPJ_BOOL opj_j2k_set_decode_area(opj_j2k_t *p_j2k,
                                 opj_image_t* p_image,
                                 OPJ_INT32 p_start_x, OPJ_INT32 p_start_y,
                                 OPJ_INT32 p_end_x, OPJ_INT32 p_end_y,
                                 opj_event_mgr_t * p_manager)
{
    opj_cp_t * l_cp = &(p_j2k->m_cp);
    opj_image_t * l_image = p_j2k->m_private_image;

    OPJ_UINT32 it_comp;
    OPJ_INT32 l_comp_x1, l_comp_y1;
    opj_image_comp_t* l_img_comp = NULL;

    /* Check if we are read the main header */
    if (p_j2k->m_specific_param.m_decoder.m_state != J2K_STATE_TPHSOT) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Need to decode the main header before begin to decode the remaining codestream");
        return OPJ_FALSE;
    }

    if (!p_start_x && !p_start_y && !p_end_x && !p_end_y) {
        opj_event_msg(p_manager, EVT_INFO,
                      "No decoded area parameters, set the decoded area to the whole image\n");

        p_j2k->m_specific_param.m_decoder.m_start_tile_x = 0;
        p_j2k->m_specific_param.m_decoder.m_start_tile_y = 0;
        p_j2k->m_specific_param.m_decoder.m_end_tile_x = l_cp->tw;
        p_j2k->m_specific_param.m_decoder.m_end_tile_y = l_cp->th;

        return OPJ_TRUE;
    }

    /* ----- */
    /* Check if the positions provided by the user are correct */

    /* Left */
    if (p_start_x < 0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Left position of the decoded area (region_x0=%d) should be >= 0.\n",
                      p_start_x);
        return OPJ_FALSE;
    } else if ((OPJ_UINT32)p_start_x > l_image->x1) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Left position of the decoded area (region_x0=%d) is outside the image area (Xsiz=%d).\n",
                      p_start_x, l_image->x1);
        return OPJ_FALSE;
    } else if ((OPJ_UINT32)p_start_x < l_image->x0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Left position of the decoded area (region_x0=%d) is outside the image area (XOsiz=%d).\n",
                      p_start_x, l_image->x0);
        p_j2k->m_specific_param.m_decoder.m_start_tile_x = 0;
        p_image->x0 = l_image->x0;
    } else {
        p_j2k->m_specific_param.m_decoder.m_start_tile_x = ((OPJ_UINT32)p_start_x -
                l_cp->tx0) / l_cp->tdx;
        p_image->x0 = (OPJ_UINT32)p_start_x;
    }

    /* Up */
    if (p_start_x < 0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Up position of the decoded area (region_y0=%d) should be >= 0.\n",
                      p_start_y);
        return OPJ_FALSE;
    } else if ((OPJ_UINT32)p_start_y > l_image->y1) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Up position of the decoded area (region_y0=%d) is outside the image area (Ysiz=%d).\n",
                      p_start_y, l_image->y1);
        return OPJ_FALSE;
    } else if ((OPJ_UINT32)p_start_y < l_image->y0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Up position of the decoded area (region_y0=%d) is outside the image area (YOsiz=%d).\n",
                      p_start_y, l_image->y0);
        p_j2k->m_specific_param.m_decoder.m_start_tile_y = 0;
        p_image->y0 = l_image->y0;
    } else {
        p_j2k->m_specific_param.m_decoder.m_start_tile_y = ((OPJ_UINT32)p_start_y -
                l_cp->ty0) / l_cp->tdy;
        p_image->y0 = (OPJ_UINT32)p_start_y;
    }

    /* Right */
    if (p_end_x <= 0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Right position of the decoded area (region_x1=%d) should be > 0.\n",
                      p_end_x);
        return OPJ_FALSE;
    } else if ((OPJ_UINT32)p_end_x < l_image->x0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Right position of the decoded area (region_x1=%d) is outside the image area (XOsiz=%d).\n",
                      p_end_x, l_image->x0);
        return OPJ_FALSE;
    } else if ((OPJ_UINT32)p_end_x > l_image->x1) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Right position of the decoded area (region_x1=%d) is outside the image area (Xsiz=%d).\n",
                      p_end_x, l_image->x1);
        p_j2k->m_specific_param.m_decoder.m_end_tile_x = l_cp->tw;
        p_image->x1 = l_image->x1;
    } else {
        p_j2k->m_specific_param.m_decoder.m_end_tile_x = (OPJ_UINT32)opj_int_ceildiv(
                    p_end_x - (OPJ_INT32)l_cp->tx0, (OPJ_INT32)l_cp->tdx);
        p_image->x1 = (OPJ_UINT32)p_end_x;
    }

    /* Bottom */
    if (p_end_y <= 0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Bottom position of the decoded area (region_y1=%d) should be > 0.\n",
                      p_end_y);
        return OPJ_FALSE;
    } else if ((OPJ_UINT32)p_end_y < l_image->y0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Bottom position of the decoded area (region_y1=%d) is outside the image area (YOsiz=%d).\n",
                      p_end_y, l_image->y0);
        return OPJ_FALSE;
    }
    if ((OPJ_UINT32)p_end_y > l_image->y1) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Bottom position of the decoded area (region_y1=%d) is outside the image area (Ysiz=%d).\n",
                      p_end_y, l_image->y1);
        p_j2k->m_specific_param.m_decoder.m_end_tile_y = l_cp->th;
        p_image->y1 = l_image->y1;
    } else {
        p_j2k->m_specific_param.m_decoder.m_end_tile_y = (OPJ_UINT32)opj_int_ceildiv(
                    p_end_y - (OPJ_INT32)l_cp->ty0, (OPJ_INT32)l_cp->tdy);
        p_image->y1 = (OPJ_UINT32)p_end_y;
    }
    /* ----- */

    p_j2k->m_specific_param.m_decoder.m_discard_tiles = 1;

    l_img_comp = p_image->comps;
    for (it_comp = 0; it_comp < p_image->numcomps; ++it_comp) {
        OPJ_INT32 l_h, l_w;

        l_img_comp->x0 = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)p_image->x0,
                         (OPJ_INT32)l_img_comp->dx);
        l_img_comp->y0 = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)p_image->y0,
                         (OPJ_INT32)l_img_comp->dy);
        l_comp_x1 = opj_int_ceildiv((OPJ_INT32)p_image->x1, (OPJ_INT32)l_img_comp->dx);
        l_comp_y1 = opj_int_ceildiv((OPJ_INT32)p_image->y1, (OPJ_INT32)l_img_comp->dy);

        l_w = opj_int_ceildivpow2(l_comp_x1, (OPJ_INT32)l_img_comp->factor)
              - opj_int_ceildivpow2((OPJ_INT32)l_img_comp->x0, (OPJ_INT32)l_img_comp->factor);
        if (l_w < 0) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Size x of the decoded component image is incorrect (comp[%d].w=%d).\n",
                          it_comp, l_w);
            return OPJ_FALSE;
        }
        l_img_comp->w = (OPJ_UINT32)l_w;

        l_h = opj_int_ceildivpow2(l_comp_y1, (OPJ_INT32)l_img_comp->factor)
              - opj_int_ceildivpow2((OPJ_INT32)l_img_comp->y0, (OPJ_INT32)l_img_comp->factor);
        if (l_h < 0) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Size y of the decoded component image is incorrect (comp[%d].h=%d).\n",
                          it_comp, l_h);
            return OPJ_FALSE;
        }
        l_img_comp->h = (OPJ_UINT32)l_h;

        l_img_comp++;
    }

    opj_event_msg(p_manager, EVT_INFO, "Setting decoding area to %d,%d,%d,%d\n",
                  p_image->x0, p_image->y0, p_image->x1, p_image->y1);

    return OPJ_TRUE;
}