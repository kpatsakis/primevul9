OPJ_BOOL opj_j2k_setup_encoder(opj_j2k_t *p_j2k,
                               opj_cparameters_t *parameters,
                               opj_image_t *image,
                               opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 i, j, tileno, numpocs_tile;
    opj_cp_t *cp = 00;

    if (!p_j2k || !parameters || ! image) {
        return OPJ_FALSE;
    }

    if ((parameters->numresolution <= 0) ||
            (parameters->numresolution > OPJ_J2K_MAXRLVLS)) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Invalid number of resolutions : %d not in range [1,%d]\n",
                      parameters->numresolution, OPJ_J2K_MAXRLVLS);
        return OPJ_FALSE;
    }

    /* keep a link to cp so that we can destroy it later in j2k_destroy_compress */
    cp = &(p_j2k->m_cp);

    /* set default values for cp */
    cp->tw = 1;
    cp->th = 1;

    /* FIXME ADE: to be removed once deprecated cp_cinema and cp_rsiz have been removed */
    if (parameters->rsiz ==
            OPJ_PROFILE_NONE) { /* consider deprecated fields only if RSIZ has not been set */
        OPJ_BOOL deprecated_used = OPJ_FALSE;
        switch (parameters->cp_cinema) {
        case OPJ_CINEMA2K_24:
            parameters->rsiz = OPJ_PROFILE_CINEMA_2K;
            parameters->max_cs_size = OPJ_CINEMA_24_CS;
            parameters->max_comp_size = OPJ_CINEMA_24_COMP;
            deprecated_used = OPJ_TRUE;
            break;
        case OPJ_CINEMA2K_48:
            parameters->rsiz = OPJ_PROFILE_CINEMA_2K;
            parameters->max_cs_size = OPJ_CINEMA_48_CS;
            parameters->max_comp_size = OPJ_CINEMA_48_COMP;
            deprecated_used = OPJ_TRUE;
            break;
        case OPJ_CINEMA4K_24:
            parameters->rsiz = OPJ_PROFILE_CINEMA_4K;
            parameters->max_cs_size = OPJ_CINEMA_24_CS;
            parameters->max_comp_size = OPJ_CINEMA_24_COMP;
            deprecated_used = OPJ_TRUE;
            break;
        case OPJ_OFF:
        default:
            break;
        }
        switch (parameters->cp_rsiz) {
        case OPJ_CINEMA2K:
            parameters->rsiz = OPJ_PROFILE_CINEMA_2K;
            deprecated_used = OPJ_TRUE;
            break;
        case OPJ_CINEMA4K:
            parameters->rsiz = OPJ_PROFILE_CINEMA_4K;
            deprecated_used = OPJ_TRUE;
            break;
        case OPJ_MCT:
            parameters->rsiz = OPJ_PROFILE_PART2 | OPJ_EXTENSION_MCT;
            deprecated_used = OPJ_TRUE;
        case OPJ_STD_RSIZ:
        default:
            break;
        }
        if (deprecated_used) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "Deprecated fields cp_cinema or cp_rsiz are used\n"
                          "Please consider using only the rsiz field\n"
                          "See openjpeg.h documentation for more details\n");
        }
    }

    /* If no explicit layers are provided, use lossless settings */
    if (parameters->tcp_numlayers == 0) {
        parameters->tcp_numlayers = 1;
        parameters->cp_disto_alloc = 1;
        parameters->tcp_rates[0] = 0;
    }

    /* see if max_codestream_size does limit input rate */
    if (parameters->max_cs_size <= 0) {
        if (parameters->tcp_rates[parameters->tcp_numlayers - 1] > 0) {
            OPJ_FLOAT32 temp_size;
            temp_size = (OPJ_FLOAT32)(image->numcomps * image->comps[0].w *
                                      image->comps[0].h * image->comps[0].prec) /
                        (parameters->tcp_rates[parameters->tcp_numlayers - 1] * 8 *
                         (OPJ_FLOAT32)image->comps[0].dx * (OPJ_FLOAT32)image->comps[0].dy);
            parameters->max_cs_size = (int) floor(temp_size);
        } else {
            parameters->max_cs_size = 0;
        }
    } else {
        OPJ_FLOAT32 temp_rate;
        OPJ_BOOL cap = OPJ_FALSE;
        temp_rate = (OPJ_FLOAT32)(image->numcomps * image->comps[0].w *
                                  image->comps[0].h * image->comps[0].prec) /
                    (OPJ_FLOAT32)(((OPJ_UINT32)parameters->max_cs_size) * 8 * image->comps[0].dx *
                                  image->comps[0].dy);
        for (i = 0; i < (OPJ_UINT32) parameters->tcp_numlayers; i++) {
            if (parameters->tcp_rates[i] < temp_rate) {
                parameters->tcp_rates[i] = temp_rate;
                cap = OPJ_TRUE;
            }
        }
        if (cap) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "The desired maximum codestream size has limited\n"
                          "at least one of the desired quality layers\n");
        }
    }

    /* Manage profiles and applications and set RSIZ */
    /* set cinema parameters if required */
    if (OPJ_IS_CINEMA(parameters->rsiz)) {
        if ((parameters->rsiz == OPJ_PROFILE_CINEMA_S2K)
                || (parameters->rsiz == OPJ_PROFILE_CINEMA_S4K)) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "JPEG 2000 Scalable Digital Cinema profiles not yet supported\n");
            parameters->rsiz = OPJ_PROFILE_NONE;
        } else {
            opj_j2k_set_cinema_parameters(parameters, image, p_manager);
            if (!opj_j2k_is_cinema_compliant(image, parameters->rsiz, p_manager)) {
                parameters->rsiz = OPJ_PROFILE_NONE;
            }
        }
    } else if (OPJ_IS_STORAGE(parameters->rsiz)) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 Long Term Storage profile not yet supported\n");
        parameters->rsiz = OPJ_PROFILE_NONE;
    } else if (OPJ_IS_BROADCAST(parameters->rsiz)) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 Broadcast profiles not yet supported\n");
        parameters->rsiz = OPJ_PROFILE_NONE;
    } else if (OPJ_IS_IMF(parameters->rsiz)) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 IMF profiles not yet supported\n");
        parameters->rsiz = OPJ_PROFILE_NONE;
    } else if (OPJ_IS_PART2(parameters->rsiz)) {
        if (parameters->rsiz == ((OPJ_PROFILE_PART2) | (OPJ_EXTENSION_NONE))) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "JPEG 2000 Part-2 profile defined\n"
                          "but no Part-2 extension enabled.\n"
                          "Profile set to NONE.\n");
            parameters->rsiz = OPJ_PROFILE_NONE;
        } else if (parameters->rsiz != ((OPJ_PROFILE_PART2) | (OPJ_EXTENSION_MCT))) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "Unsupported Part-2 extension enabled\n"
                          "Profile set to NONE.\n");
            parameters->rsiz = OPJ_PROFILE_NONE;
        }
    }

    /*
    copy user encoding parameters
    */
    cp->m_specific_param.m_enc.m_max_comp_size = (OPJ_UINT32)
            parameters->max_comp_size;
    cp->rsiz = parameters->rsiz;
    cp->m_specific_param.m_enc.m_disto_alloc = (OPJ_UINT32)
            parameters->cp_disto_alloc & 1u;
    cp->m_specific_param.m_enc.m_fixed_alloc = (OPJ_UINT32)
            parameters->cp_fixed_alloc & 1u;
    cp->m_specific_param.m_enc.m_fixed_quality = (OPJ_UINT32)
            parameters->cp_fixed_quality & 1u;

    /* mod fixed_quality */
    if (parameters->cp_fixed_alloc && parameters->cp_matrice) {
        size_t array_size = (size_t)parameters->tcp_numlayers *
                            (size_t)parameters->numresolution * 3 * sizeof(OPJ_INT32);
        cp->m_specific_param.m_enc.m_matrice = (OPJ_INT32 *) opj_malloc(array_size);
        if (!cp->m_specific_param.m_enc.m_matrice) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Not enough memory to allocate copy of user encoding parameters matrix \n");
            return OPJ_FALSE;
        }
        memcpy(cp->m_specific_param.m_enc.m_matrice, parameters->cp_matrice,
               array_size);
    }

    /* tiles */
    cp->tdx = (OPJ_UINT32)parameters->cp_tdx;
    cp->tdy = (OPJ_UINT32)parameters->cp_tdy;

    /* tile offset */
    cp->tx0 = (OPJ_UINT32)parameters->cp_tx0;
    cp->ty0 = (OPJ_UINT32)parameters->cp_ty0;

    /* comment string */
    if (parameters->cp_comment) {
        cp->comment = (char*)opj_malloc(strlen(parameters->cp_comment) + 1U);
        if (!cp->comment) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Not enough memory to allocate copy of comment string\n");
            return OPJ_FALSE;
        }
        strcpy(cp->comment, parameters->cp_comment);
    } else {
        /* Create default comment for codestream */
        const char comment[] = "Created by OpenJPEG version ";
        const size_t clen = strlen(comment);
        const char *version = opj_version();

        /* UniPG>> */
#ifdef USE_JPWL
        cp->comment = (char*)opj_malloc(clen + strlen(version) + 11);
        if (!cp->comment) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Not enough memory to allocate comment string\n");
            return OPJ_FALSE;
        }
        sprintf(cp->comment, "%s%s with JPWL", comment, version);
#else
        cp->comment = (char*)opj_malloc(clen + strlen(version) + 1);
        if (!cp->comment) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Not enough memory to allocate comment string\n");
            return OPJ_FALSE;
        }
        sprintf(cp->comment, "%s%s", comment, version);
#endif
        /* <<UniPG */
    }

    /*
    calculate other encoding parameters
    */

    if (parameters->tile_size_on) {
        cp->tw = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)(image->x1 - cp->tx0),
                                             (OPJ_INT32)cp->tdx);
        cp->th = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)(image->y1 - cp->ty0),
                                             (OPJ_INT32)cp->tdy);
    } else {
        cp->tdx = image->x1 - cp->tx0;
        cp->tdy = image->y1 - cp->ty0;
    }

    if (parameters->tp_on) {
        cp->m_specific_param.m_enc.m_tp_flag = (OPJ_BYTE)parameters->tp_flag;
        cp->m_specific_param.m_enc.m_tp_on = 1;
    }

#ifdef USE_JPWL
    /*
    calculate JPWL encoding parameters
    */

    if (parameters->jpwl_epc_on) {
        OPJ_INT32 i;

        /* set JPWL on */
        cp->epc_on = OPJ_TRUE;
        cp->info_on = OPJ_FALSE; /* no informative technique */

        /* set EPB on */
        if ((parameters->jpwl_hprot_MH > 0) || (parameters->jpwl_hprot_TPH[0] > 0)) {
            cp->epb_on = OPJ_TRUE;

            cp->hprot_MH = parameters->jpwl_hprot_MH;
            for (i = 0; i < JPWL_MAX_NO_TILESPECS; i++) {
                cp->hprot_TPH_tileno[i] = parameters->jpwl_hprot_TPH_tileno[i];
                cp->hprot_TPH[i] = parameters->jpwl_hprot_TPH[i];
            }
            /* if tile specs are not specified, copy MH specs */
            if (cp->hprot_TPH[0] == -1) {
                cp->hprot_TPH_tileno[0] = 0;
                cp->hprot_TPH[0] = parameters->jpwl_hprot_MH;
            }
            for (i = 0; i < JPWL_MAX_NO_PACKSPECS; i++) {
                cp->pprot_tileno[i] = parameters->jpwl_pprot_tileno[i];
                cp->pprot_packno[i] = parameters->jpwl_pprot_packno[i];
                cp->pprot[i] = parameters->jpwl_pprot[i];
            }
        }

        /* set ESD writing */
        if ((parameters->jpwl_sens_size == 1) || (parameters->jpwl_sens_size == 2)) {
            cp->esd_on = OPJ_TRUE;

            cp->sens_size = parameters->jpwl_sens_size;
            cp->sens_addr = parameters->jpwl_sens_addr;
            cp->sens_range = parameters->jpwl_sens_range;

            cp->sens_MH = parameters->jpwl_sens_MH;
            for (i = 0; i < JPWL_MAX_NO_TILESPECS; i++) {
                cp->sens_TPH_tileno[i] = parameters->jpwl_sens_TPH_tileno[i];
                cp->sens_TPH[i] = parameters->jpwl_sens_TPH[i];
            }
        }

        /* always set RED writing to false: we are at the encoder */
        cp->red_on = OPJ_FALSE;

    } else {
        cp->epc_on = OPJ_FALSE;
    }
#endif /* USE_JPWL */

    /* initialize the mutiple tiles */
    /* ---------------------------- */
    cp->tcps = (opj_tcp_t*) opj_calloc(cp->tw * cp->th, sizeof(opj_tcp_t));
    if (!cp->tcps) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory to allocate tile coding parameters\n");
        return OPJ_FALSE;
    }
    if (parameters->numpocs) {
        /* initialisation of POC */
        opj_j2k_check_poc_val(parameters->POC, parameters->numpocs,
                              (OPJ_UINT32)parameters->numresolution, image->numcomps,
                              (OPJ_UINT32)parameters->tcp_numlayers, p_manager);
        /* TODO MSD use the return value*/
    }

    for (tileno = 0; tileno < cp->tw * cp->th; tileno++) {
        opj_tcp_t *tcp = &cp->tcps[tileno];
        tcp->numlayers = (OPJ_UINT32)parameters->tcp_numlayers;

        for (j = 0; j < tcp->numlayers; j++) {
            if (OPJ_IS_CINEMA(cp->rsiz)) {
                if (cp->m_specific_param.m_enc.m_fixed_quality) {
                    tcp->distoratio[j] = parameters->tcp_distoratio[j];
                }
                tcp->rates[j] = parameters->tcp_rates[j];
            } else {
                if (cp->m_specific_param.m_enc.m_fixed_quality) {       /* add fixed_quality */
                    tcp->distoratio[j] = parameters->tcp_distoratio[j];
                } else {
                    tcp->rates[j] = parameters->tcp_rates[j];
                }
            }
        }

        tcp->csty = (OPJ_UINT32)parameters->csty;
        tcp->prg = parameters->prog_order;
        tcp->mct = (OPJ_UINT32)parameters->tcp_mct;

        numpocs_tile = 0;
        tcp->POC = 0;

        if (parameters->numpocs) {
            /* initialisation of POC */
            tcp->POC = 1;
            for (i = 0; i < parameters->numpocs; i++) {
                if (tileno + 1 == parameters->POC[i].tile)  {
                    opj_poc_t *tcp_poc = &tcp->pocs[numpocs_tile];

                    tcp_poc->resno0         = parameters->POC[numpocs_tile].resno0;
                    tcp_poc->compno0        = parameters->POC[numpocs_tile].compno0;
                    tcp_poc->layno1         = parameters->POC[numpocs_tile].layno1;
                    tcp_poc->resno1         = parameters->POC[numpocs_tile].resno1;
                    tcp_poc->compno1        = parameters->POC[numpocs_tile].compno1;
                    tcp_poc->prg1           = parameters->POC[numpocs_tile].prg1;
                    tcp_poc->tile           = parameters->POC[numpocs_tile].tile;

                    numpocs_tile++;
                }
            }

            tcp->numpocs = numpocs_tile - 1 ;
        } else {
            tcp->numpocs = 0;
        }

        tcp->tccps = (opj_tccp_t*) opj_calloc(image->numcomps, sizeof(opj_tccp_t));
        if (!tcp->tccps) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Not enough memory to allocate tile component coding parameters\n");
            return OPJ_FALSE;
        }
        if (parameters->mct_data) {

            OPJ_UINT32 lMctSize = image->numcomps * image->numcomps * (OPJ_UINT32)sizeof(
                                      OPJ_FLOAT32);
            OPJ_FLOAT32 * lTmpBuf = (OPJ_FLOAT32*)opj_malloc(lMctSize);
            OPJ_INT32 * l_dc_shift = (OPJ_INT32 *)((OPJ_BYTE *) parameters->mct_data +
                                                   lMctSize);

            if (!lTmpBuf) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Not enough memory to allocate temp buffer\n");
                return OPJ_FALSE;
            }

            tcp->mct = 2;
            tcp->m_mct_coding_matrix = (OPJ_FLOAT32*)opj_malloc(lMctSize);
            if (! tcp->m_mct_coding_matrix) {
                opj_free(lTmpBuf);
                lTmpBuf = NULL;
                opj_event_msg(p_manager, EVT_ERROR,
                              "Not enough memory to allocate encoder MCT coding matrix \n");
                return OPJ_FALSE;
            }
            memcpy(tcp->m_mct_coding_matrix, parameters->mct_data, lMctSize);
            memcpy(lTmpBuf, parameters->mct_data, lMctSize);

            tcp->m_mct_decoding_matrix = (OPJ_FLOAT32*)opj_malloc(lMctSize);
            if (! tcp->m_mct_decoding_matrix) {
                opj_free(lTmpBuf);
                lTmpBuf = NULL;
                opj_event_msg(p_manager, EVT_ERROR,
                              "Not enough memory to allocate encoder MCT decoding matrix \n");
                return OPJ_FALSE;
            }
            if (opj_matrix_inversion_f(lTmpBuf, (tcp->m_mct_decoding_matrix),
                                       image->numcomps) == OPJ_FALSE) {
                opj_free(lTmpBuf);
                lTmpBuf = NULL;
                opj_event_msg(p_manager, EVT_ERROR,
                              "Failed to inverse encoder MCT decoding matrix \n");
                return OPJ_FALSE;
            }

            tcp->mct_norms = (OPJ_FLOAT64*)
                             opj_malloc(image->numcomps * sizeof(OPJ_FLOAT64));
            if (! tcp->mct_norms) {
                opj_free(lTmpBuf);
                lTmpBuf = NULL;
                opj_event_msg(p_manager, EVT_ERROR,
                              "Not enough memory to allocate encoder MCT norms \n");
                return OPJ_FALSE;
            }
            opj_calculate_norms(tcp->mct_norms, image->numcomps,
                                tcp->m_mct_decoding_matrix);
            opj_free(lTmpBuf);

            for (i = 0; i < image->numcomps; i++) {
                opj_tccp_t *tccp = &tcp->tccps[i];
                tccp->m_dc_level_shift = l_dc_shift[i];
            }

            if (opj_j2k_setup_mct_encoding(tcp, image) == OPJ_FALSE) {
                /* free will be handled by opj_j2k_destroy */
                opj_event_msg(p_manager, EVT_ERROR, "Failed to setup j2k mct encoding\n");
                return OPJ_FALSE;
            }
        } else {
            if (tcp->mct == 1 && image->numcomps >= 3) { /* RGB->YCC MCT is enabled */
                if ((image->comps[0].dx != image->comps[1].dx) ||
                        (image->comps[0].dx != image->comps[2].dx) ||
                        (image->comps[0].dy != image->comps[1].dy) ||
                        (image->comps[0].dy != image->comps[2].dy)) {
                    opj_event_msg(p_manager, EVT_WARNING,
                                  "Cannot perform MCT on components with different sizes. Disabling MCT.\n");
                    tcp->mct = 0;
                }
            }
            for (i = 0; i < image->numcomps; i++) {
                opj_tccp_t *tccp = &tcp->tccps[i];
                opj_image_comp_t * l_comp = &(image->comps[i]);

                if (! l_comp->sgnd) {
                    tccp->m_dc_level_shift = 1 << (l_comp->prec - 1);
                }
            }
        }

        for (i = 0; i < image->numcomps; i++) {
            opj_tccp_t *tccp = &tcp->tccps[i];

            tccp->csty = parameters->csty &
                         0x01;   /* 0 => one precinct || 1 => custom precinct  */
            tccp->numresolutions = (OPJ_UINT32)parameters->numresolution;
            tccp->cblkw = (OPJ_UINT32)opj_int_floorlog2(parameters->cblockw_init);
            tccp->cblkh = (OPJ_UINT32)opj_int_floorlog2(parameters->cblockh_init);
            tccp->cblksty = (OPJ_UINT32)parameters->mode;
            tccp->qmfbid = parameters->irreversible ? 0 : 1;
            tccp->qntsty = parameters->irreversible ? J2K_CCP_QNTSTY_SEQNT :
                           J2K_CCP_QNTSTY_NOQNT;
            tccp->numgbits = 2;

            if ((OPJ_INT32)i == parameters->roi_compno) {
                tccp->roishift = parameters->roi_shift;
            } else {
                tccp->roishift = 0;
            }

            if (parameters->csty & J2K_CCP_CSTY_PRT) {
                OPJ_INT32 p = 0, it_res;
                assert(tccp->numresolutions > 0);
                for (it_res = (OPJ_INT32)tccp->numresolutions - 1; it_res >= 0; it_res--) {
                    if (p < parameters->res_spec) {

                        if (parameters->prcw_init[p] < 1) {
                            tccp->prcw[it_res] = 1;
                        } else {
                            tccp->prcw[it_res] = (OPJ_UINT32)opj_int_floorlog2(parameters->prcw_init[p]);
                        }

                        if (parameters->prch_init[p] < 1) {
                            tccp->prch[it_res] = 1;
                        } else {
                            tccp->prch[it_res] = (OPJ_UINT32)opj_int_floorlog2(parameters->prch_init[p]);
                        }

                    } else {
                        OPJ_INT32 res_spec = parameters->res_spec;
                        OPJ_INT32 size_prcw = 0;
                        OPJ_INT32 size_prch = 0;

                        assert(res_spec > 0); /* issue 189 */
                        size_prcw = parameters->prcw_init[res_spec - 1] >> (p - (res_spec - 1));
                        size_prch = parameters->prch_init[res_spec - 1] >> (p - (res_spec - 1));


                        if (size_prcw < 1) {
                            tccp->prcw[it_res] = 1;
                        } else {
                            tccp->prcw[it_res] = (OPJ_UINT32)opj_int_floorlog2(size_prcw);
                        }

                        if (size_prch < 1) {
                            tccp->prch[it_res] = 1;
                        } else {
                            tccp->prch[it_res] = (OPJ_UINT32)opj_int_floorlog2(size_prch);
                        }
                    }
                    p++;
                    /*printf("\nsize precinct for level %d : %d,%d\n", it_res,tccp->prcw[it_res], tccp->prch[it_res]); */
                }       /*end for*/
            } else {
                for (j = 0; j < tccp->numresolutions; j++) {
                    tccp->prcw[j] = 15;
                    tccp->prch[j] = 15;
                }
            }

            opj_dwt_calc_explicit_stepsizes(tccp, image->comps[i].prec);
        }
    }

    if (parameters->mct_data) {
        opj_free(parameters->mct_data);
        parameters->mct_data = 00;
    }
    return OPJ_TRUE;
}