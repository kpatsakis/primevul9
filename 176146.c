static OPJ_BOOL opj_j2k_read_siz(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 i;
    OPJ_UINT32 l_nb_comp;
    OPJ_UINT32 l_nb_comp_remain;
    OPJ_UINT32 l_remaining_size;
    OPJ_UINT32 l_nb_tiles;
    OPJ_UINT32 l_tmp, l_tx1, l_ty1;
    OPJ_UINT32 l_prec0, l_sgnd0;
    opj_image_t *l_image = 00;
    opj_cp_t *l_cp = 00;
    opj_image_comp_t * l_img_comp = 00;
    opj_tcp_t * l_current_tile_param = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_header_data != 00);

    l_image = p_j2k->m_private_image;
    l_cp = &(p_j2k->m_cp);

    /* minimum size == 39 - 3 (= minimum component parameter) */
    if (p_header_size < 36) {
        opj_event_msg(p_manager, EVT_ERROR, "Error with SIZ marker size\n");
        return OPJ_FALSE;
    }

    l_remaining_size = p_header_size - 36;
    l_nb_comp = l_remaining_size / 3;
    l_nb_comp_remain = l_remaining_size % 3;
    if (l_nb_comp_remain != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error with SIZ marker size\n");
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data, &l_tmp,
                   2);                                                /* Rsiz (capabilities) */
    p_header_data += 2;
    l_cp->rsiz = (OPJ_UINT16) l_tmp;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_image->x1, 4);   /* Xsiz */
    p_header_data += 4;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_image->y1, 4);   /* Ysiz */
    p_header_data += 4;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_image->x0, 4);   /* X0siz */
    p_header_data += 4;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_image->y0, 4);   /* Y0siz */
    p_header_data += 4;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_cp->tdx,
                   4);             /* XTsiz */
    p_header_data += 4;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_cp->tdy,
                   4);             /* YTsiz */
    p_header_data += 4;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_cp->tx0,
                   4);             /* XT0siz */
    p_header_data += 4;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_cp->ty0,
                   4);             /* YT0siz */
    p_header_data += 4;
    opj_read_bytes(p_header_data, (OPJ_UINT32*) &l_tmp,
                   2);                 /* Csiz */
    p_header_data += 2;
    if (l_tmp < 16385) {
        l_image->numcomps = (OPJ_UINT16) l_tmp;
    } else {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Error with SIZ marker: number of component is illegal -> %d\n", l_tmp);
        return OPJ_FALSE;
    }

    if (l_image->numcomps != l_nb_comp) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Error with SIZ marker: number of component is not compatible with the remaining number of parameters ( %d vs %d)\n",
                      l_image->numcomps, l_nb_comp);
        return OPJ_FALSE;
    }

    /* testcase 4035.pdf.SIGSEGV.d8b.3375 */
    /* testcase issue427-null-image-size.jp2 */
    if ((l_image->x0 >= l_image->x1) || (l_image->y0 >= l_image->y1)) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Error with SIZ marker: negative or zero image size (%" PRId64 " x %" PRId64
                      ")\n", (OPJ_INT64)l_image->x1 - l_image->x0,
                      (OPJ_INT64)l_image->y1 - l_image->y0);
        return OPJ_FALSE;
    }
    /* testcase 2539.pdf.SIGFPE.706.1712 (also 3622.pdf.SIGFPE.706.2916 and 4008.pdf.SIGFPE.706.3345 and maybe more) */
    if ((l_cp->tdx == 0U) || (l_cp->tdy == 0U)) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Error with SIZ marker: invalid tile size (tdx: %d, tdy: %d)\n", l_cp->tdx,
                      l_cp->tdy);
        return OPJ_FALSE;
    }

    /* testcase 1610.pdf.SIGSEGV.59c.681 */
    if ((0xFFFFFFFFU / l_image->x1) < l_image->y1) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Prevent buffer overflow (x1: %d, y1: %d)\n", l_image->x1, l_image->y1);
        return OPJ_FALSE;
    }

    /* testcase issue427-illegal-tile-offset.jp2 */
    l_tx1 = opj_uint_adds(l_cp->tx0, l_cp->tdx); /* manage overflow */
    l_ty1 = opj_uint_adds(l_cp->ty0, l_cp->tdy); /* manage overflow */
    if ((l_cp->tx0 > l_image->x0) || (l_cp->ty0 > l_image->y0) ||
            (l_tx1 <= l_image->x0) || (l_ty1 <= l_image->y0)) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Error with SIZ marker: illegal tile offset\n");
        return OPJ_FALSE;
    }
    if (!p_j2k->dump_state) {
        OPJ_UINT32 siz_w, siz_h;

        siz_w = l_image->x1 - l_image->x0;
        siz_h = l_image->y1 - l_image->y0;

        if (p_j2k->ihdr_w > 0 && p_j2k->ihdr_h > 0
                && (p_j2k->ihdr_w != siz_w || p_j2k->ihdr_h != siz_h)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Error with SIZ marker: IHDR w(%u) h(%u) vs. SIZ w(%u) h(%u)\n", p_j2k->ihdr_w,
                          p_j2k->ihdr_h, siz_w, siz_h);
            return OPJ_FALSE;
        }
    }
#ifdef USE_JPWL
    if (l_cp->correct) {
        /* if JPWL is on, we check whether TX errors have damaged
          too much the SIZ parameters */
        if (!(l_image->x1 * l_image->y1)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "JPWL: bad image size (%d x %d)\n",
                          l_image->x1, l_image->y1);
            if (!JPWL_ASSUME) {
                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                return OPJ_FALSE;
            }
        }

        /* FIXME check previously in the function so why keep this piece of code ? Need by the norm ?
                if (l_image->numcomps != ((len - 38) / 3)) {
                        opj_event_msg(p_manager, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR,
                                "JPWL: Csiz is %d => space in SIZ only for %d comps.!!!\n",
                                l_image->numcomps, ((len - 38) / 3));
                        if (!JPWL_ASSUME) {
                                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                                return OPJ_FALSE;
                        }
        */              /* we try to correct */
        /*              opj_event_msg(p_manager, EVT_WARNING, "- trying to adjust this\n");
                        if (l_image->numcomps < ((len - 38) / 3)) {
                                len = 38 + 3 * l_image->numcomps;
                                opj_event_msg(p_manager, EVT_WARNING, "- setting Lsiz to %d => HYPOTHESIS!!!\n",
                                        len);
                        } else {
                                l_image->numcomps = ((len - 38) / 3);
                                opj_event_msg(p_manager, EVT_WARNING, "- setting Csiz to %d => HYPOTHESIS!!!\n",
                                        l_image->numcomps);
                        }
                }
        */

        /* update components number in the jpwl_exp_comps filed */
        l_cp->exp_comps = l_image->numcomps;
    }
#endif /* USE_JPWL */

    /* Allocate the resulting image components */
    l_image->comps = (opj_image_comp_t*) opj_calloc(l_image->numcomps,
                     sizeof(opj_image_comp_t));
    if (l_image->comps == 00) {
        l_image->numcomps = 0;
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory to take in charge SIZ marker\n");
        return OPJ_FALSE;
    }

    l_img_comp = l_image->comps;

    l_prec0 = 0;
    l_sgnd0 = 0;
    /* Read the component information */
    for (i = 0; i < l_image->numcomps; ++i) {
        OPJ_UINT32 tmp;
        opj_read_bytes(p_header_data, &tmp, 1); /* Ssiz_i */
        ++p_header_data;
        l_img_comp->prec = (tmp & 0x7f) + 1;
        l_img_comp->sgnd = tmp >> 7;

        if (p_j2k->dump_state == 0) {
            if (i == 0) {
                l_prec0 = l_img_comp->prec;
                l_sgnd0 = l_img_comp->sgnd;
            } else if (!l_cp->allow_different_bit_depth_sign
                       && (l_img_comp->prec != l_prec0 || l_img_comp->sgnd != l_sgnd0)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "Despite JP2 BPC!=255, precision and/or sgnd values for comp[%d] is different than comp[0]:\n"
                              "        [0] prec(%d) sgnd(%d) [%d] prec(%d) sgnd(%d)\n", i, l_prec0, l_sgnd0,
                              i, l_img_comp->prec, l_img_comp->sgnd);
            }
            /* TODO: we should perhaps also check against JP2 BPCC values */
        }
        opj_read_bytes(p_header_data, &tmp, 1); /* XRsiz_i */
        ++p_header_data;
        l_img_comp->dx = (OPJ_UINT32)tmp; /* should be between 1 and 255 */
        opj_read_bytes(p_header_data, &tmp, 1); /* YRsiz_i */
        ++p_header_data;
        l_img_comp->dy = (OPJ_UINT32)tmp; /* should be between 1 and 255 */
        if (l_img_comp->dx < 1 || l_img_comp->dx > 255 ||
                l_img_comp->dy < 1 || l_img_comp->dy > 255) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Invalid values for comp = %d : dx=%u dy=%u (should be between 1 and 255 according to the JPEG2000 norm)\n",
                          i, l_img_comp->dx, l_img_comp->dy);
            return OPJ_FALSE;
        }
        /* Avoids later undefined shift in computation of */
        /* p_j2k->m_specific_param.m_decoder.m_default_tcp->tccps[i].m_dc_level_shift = 1
                    << (l_image->comps[i].prec - 1); */
        if (l_img_comp->prec > 31) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Invalid values for comp = %d : prec=%u (should be between 1 and 38 according to the JPEG2000 norm. OpenJpeg only supports up to 31)\n",
                          i, l_img_comp->prec);
            return OPJ_FALSE;
        }
#ifdef USE_JPWL
        if (l_cp->correct) {
            /* if JPWL is on, we check whether TX errors have damaged
                    too much the SIZ parameters, again */
            if (!(l_image->comps[i].dx * l_image->comps[i].dy)) {
                opj_event_msg(p_manager, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR,
                              "JPWL: bad XRsiz_%d/YRsiz_%d (%d x %d)\n",
                              i, i, l_image->comps[i].dx, l_image->comps[i].dy);
                if (!JPWL_ASSUME) {
                    opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                    return OPJ_FALSE;
                }
                /* we try to correct */
                opj_event_msg(p_manager, EVT_WARNING, "- trying to adjust them\n");
                if (!l_image->comps[i].dx) {
                    l_image->comps[i].dx = 1;
                    opj_event_msg(p_manager, EVT_WARNING,
                                  "- setting XRsiz_%d to %d => HYPOTHESIS!!!\n",
                                  i, l_image->comps[i].dx);
                }
                if (!l_image->comps[i].dy) {
                    l_image->comps[i].dy = 1;
                    opj_event_msg(p_manager, EVT_WARNING,
                                  "- setting YRsiz_%d to %d => HYPOTHESIS!!!\n",
                                  i, l_image->comps[i].dy);
                }
            }
        }
#endif /* USE_JPWL */
        l_img_comp->resno_decoded =
            0;                                                          /* number of resolution decoded */
        l_img_comp->factor =
            l_cp->m_specific_param.m_dec.m_reduce; /* reducing factor per component */
        ++l_img_comp;
    }

    if (l_cp->tdx == 0 || l_cp->tdy == 0) {
        return OPJ_FALSE;
    }

    /* Compute the number of tiles */
    l_cp->tw = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)(l_image->x1 - l_cp->tx0),
                                           (OPJ_INT32)l_cp->tdx);
    l_cp->th = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)(l_image->y1 - l_cp->ty0),
                                           (OPJ_INT32)l_cp->tdy);

    /* Check that the number of tiles is valid */
    if (l_cp->tw == 0 || l_cp->th == 0 || l_cp->tw > 65535 / l_cp->th) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Invalid number of tiles : %u x %u (maximum fixed by jpeg2000 norm is 65535 tiles)\n",
                      l_cp->tw, l_cp->th);
        return OPJ_FALSE;
    }
    l_nb_tiles = l_cp->tw * l_cp->th;

    /* Define the tiles which will be decoded */
    if (p_j2k->m_specific_param.m_decoder.m_discard_tiles) {
        p_j2k->m_specific_param.m_decoder.m_start_tile_x =
            (p_j2k->m_specific_param.m_decoder.m_start_tile_x - l_cp->tx0) / l_cp->tdx;
        p_j2k->m_specific_param.m_decoder.m_start_tile_y =
            (p_j2k->m_specific_param.m_decoder.m_start_tile_y - l_cp->ty0) / l_cp->tdy;
        p_j2k->m_specific_param.m_decoder.m_end_tile_x = (OPJ_UINT32)opj_int_ceildiv((
                    OPJ_INT32)(p_j2k->m_specific_param.m_decoder.m_end_tile_x - l_cp->tx0),
                (OPJ_INT32)l_cp->tdx);
        p_j2k->m_specific_param.m_decoder.m_end_tile_y = (OPJ_UINT32)opj_int_ceildiv((
                    OPJ_INT32)(p_j2k->m_specific_param.m_decoder.m_end_tile_y - l_cp->ty0),
                (OPJ_INT32)l_cp->tdy);
    } else {
        p_j2k->m_specific_param.m_decoder.m_start_tile_x = 0;
        p_j2k->m_specific_param.m_decoder.m_start_tile_y = 0;
        p_j2k->m_specific_param.m_decoder.m_end_tile_x = l_cp->tw;
        p_j2k->m_specific_param.m_decoder.m_end_tile_y = l_cp->th;
    }

#ifdef USE_JPWL
    if (l_cp->correct) {
        /* if JPWL is on, we check whether TX errors have damaged
          too much the SIZ parameters */
        if ((l_cp->tw < 1) || (l_cp->th < 1) || (l_cp->tw > l_cp->max_tiles) ||
                (l_cp->th > l_cp->max_tiles)) {
            opj_event_msg(p_manager, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR,
                          "JPWL: bad number of tiles (%d x %d)\n",
                          l_cp->tw, l_cp->th);
            if (!JPWL_ASSUME) {
                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                return OPJ_FALSE;
            }
            /* we try to correct */
            opj_event_msg(p_manager, EVT_WARNING, "- trying to adjust them\n");
            if (l_cp->tw < 1) {
                l_cp->tw = 1;
                opj_event_msg(p_manager, EVT_WARNING,
                              "- setting %d tiles in x => HYPOTHESIS!!!\n",
                              l_cp->tw);
            }
            if (l_cp->tw > l_cp->max_tiles) {
                l_cp->tw = 1;
                opj_event_msg(p_manager, EVT_WARNING,
                              "- too large x, increase expectance of %d\n"
                              "- setting %d tiles in x => HYPOTHESIS!!!\n",
                              l_cp->max_tiles, l_cp->tw);
            }
            if (l_cp->th < 1) {
                l_cp->th = 1;
                opj_event_msg(p_manager, EVT_WARNING,
                              "- setting %d tiles in y => HYPOTHESIS!!!\n",
                              l_cp->th);
            }
            if (l_cp->th > l_cp->max_tiles) {
                l_cp->th = 1;
                opj_event_msg(p_manager, EVT_WARNING,
                              "- too large y, increase expectance of %d to continue\n",
                              "- setting %d tiles in y => HYPOTHESIS!!!\n",
                              l_cp->max_tiles, l_cp->th);
            }
        }
    }
#endif /* USE_JPWL */

    /* memory allocations */
    l_cp->tcps = (opj_tcp_t*) opj_calloc(l_nb_tiles, sizeof(opj_tcp_t));
    if (l_cp->tcps == 00) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory to take in charge SIZ marker\n");
        return OPJ_FALSE;
    }

#ifdef USE_JPWL
    if (l_cp->correct) {
        if (!l_cp->tcps) {
            opj_event_msg(p_manager, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR,
                          "JPWL: could not alloc tcps field of cp\n");
            if (!JPWL_ASSUME) {
                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                return OPJ_FALSE;
            }
        }
    }
#endif /* USE_JPWL */

    p_j2k->m_specific_param.m_decoder.m_default_tcp->tccps =
        (opj_tccp_t*) opj_calloc(l_image->numcomps, sizeof(opj_tccp_t));
    if (p_j2k->m_specific_param.m_decoder.m_default_tcp->tccps  == 00) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory to take in charge SIZ marker\n");
        return OPJ_FALSE;
    }

    p_j2k->m_specific_param.m_decoder.m_default_tcp->m_mct_records =
        (opj_mct_data_t*)opj_calloc(OPJ_J2K_MCT_DEFAULT_NB_RECORDS,
                                    sizeof(opj_mct_data_t));

    if (! p_j2k->m_specific_param.m_decoder.m_default_tcp->m_mct_records) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory to take in charge SIZ marker\n");
        return OPJ_FALSE;
    }
    p_j2k->m_specific_param.m_decoder.m_default_tcp->m_nb_max_mct_records =
        OPJ_J2K_MCT_DEFAULT_NB_RECORDS;

    p_j2k->m_specific_param.m_decoder.m_default_tcp->m_mcc_records =
        (opj_simple_mcc_decorrelation_data_t*)
        opj_calloc(OPJ_J2K_MCC_DEFAULT_NB_RECORDS,
                   sizeof(opj_simple_mcc_decorrelation_data_t));

    if (! p_j2k->m_specific_param.m_decoder.m_default_tcp->m_mcc_records) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory to take in charge SIZ marker\n");
        return OPJ_FALSE;
    }
    p_j2k->m_specific_param.m_decoder.m_default_tcp->m_nb_max_mcc_records =
        OPJ_J2K_MCC_DEFAULT_NB_RECORDS;

    /* set up default dc level shift */
    for (i = 0; i < l_image->numcomps; ++i) {
        if (! l_image->comps[i].sgnd) {
            p_j2k->m_specific_param.m_decoder.m_default_tcp->tccps[i].m_dc_level_shift = 1
                    << (l_image->comps[i].prec - 1);
        }
    }

    l_current_tile_param = l_cp->tcps;
    for (i = 0; i < l_nb_tiles; ++i) {
        l_current_tile_param->tccps = (opj_tccp_t*) opj_calloc(l_image->numcomps,
                                      sizeof(opj_tccp_t));
        if (l_current_tile_param->tccps == 00) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Not enough memory to take in charge SIZ marker\n");
            return OPJ_FALSE;
        }

        ++l_current_tile_param;
    }

    p_j2k->m_specific_param.m_decoder.m_state = J2K_STATE_MH;
    opj_image_comp_header_update(l_image, l_cp);

    return OPJ_TRUE;
}