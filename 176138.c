static OPJ_BOOL opj_j2k_update_image_data(opj_tcd_t * p_tcd, OPJ_BYTE * p_data,
        opj_image_t* p_output_image)
{
    OPJ_UINT32 i, j, k = 0;
    OPJ_UINT32 l_width_src, l_height_src;
    OPJ_UINT32 l_width_dest, l_height_dest;
    OPJ_INT32 l_offset_x0_src, l_offset_y0_src, l_offset_x1_src, l_offset_y1_src;
    OPJ_SIZE_T l_start_offset_src, l_line_offset_src, l_end_offset_src ;
    OPJ_UINT32 l_start_x_dest, l_start_y_dest;
    OPJ_UINT32 l_x0_dest, l_y0_dest, l_x1_dest, l_y1_dest;
    OPJ_SIZE_T l_start_offset_dest, l_line_offset_dest;

    opj_image_comp_t * l_img_comp_src = 00;
    opj_image_comp_t * l_img_comp_dest = 00;

    opj_tcd_tilecomp_t * l_tilec = 00;
    opj_image_t * l_image_src = 00;
    OPJ_UINT32 l_size_comp, l_remaining;
    OPJ_INT32 * l_dest_ptr;
    opj_tcd_resolution_t* l_res = 00;

    l_tilec = p_tcd->tcd_image->tiles->comps;
    l_image_src = p_tcd->image;
    l_img_comp_src = l_image_src->comps;

    l_img_comp_dest = p_output_image->comps;

    for (i = 0; i < l_image_src->numcomps; i++) {

        /* Allocate output component buffer if necessary */
        if (!l_img_comp_dest->data) {
            OPJ_SIZE_T l_width = l_img_comp_dest->w;
            OPJ_SIZE_T l_height = l_img_comp_dest->h;

            if ((l_height == 0U) || (l_width > (SIZE_MAX / l_height)) ||
                    l_width * l_height > SIZE_MAX / sizeof(OPJ_INT32)) {
                /* would overflow */
                return OPJ_FALSE;
            }
            l_img_comp_dest->data = (OPJ_INT32*) opj_image_data_alloc(l_width * l_height *
                                    sizeof(OPJ_INT32));
            if (! l_img_comp_dest->data) {
                return OPJ_FALSE;
            }
            /* Do we really need this memset ? */
            memset(l_img_comp_dest->data, 0, l_width * l_height * sizeof(OPJ_INT32));
        }

        /* Copy info from decoded comp image to output image */
        l_img_comp_dest->resno_decoded = l_img_comp_src->resno_decoded;

        /*-----*/
        /* Compute the precision of the output buffer */
        l_size_comp = l_img_comp_src->prec >> 3; /*(/ 8)*/
        l_remaining = l_img_comp_src->prec & 7;  /* (%8) */
        l_res = l_tilec->resolutions + l_img_comp_src->resno_decoded;

        if (l_remaining) {
            ++l_size_comp;
        }

        if (l_size_comp == 3) {
            l_size_comp = 4;
        }
        /*-----*/

        /* Current tile component size*/
        /*if (i == 0) {
        fprintf(stdout, "SRC: l_res_x0=%d, l_res_x1=%d, l_res_y0=%d, l_res_y1=%d\n",
                        l_res->x0, l_res->x1, l_res->y0, l_res->y1);
        }*/

        l_width_src = (OPJ_UINT32)(l_res->x1 - l_res->x0);
        l_height_src = (OPJ_UINT32)(l_res->y1 - l_res->y0);

        /* Border of the current output component*/
        l_x0_dest = opj_uint_ceildivpow2(l_img_comp_dest->x0, l_img_comp_dest->factor);
        l_y0_dest = opj_uint_ceildivpow2(l_img_comp_dest->y0, l_img_comp_dest->factor);
        l_x1_dest = l_x0_dest +
                    l_img_comp_dest->w; /* can't overflow given that image->x1 is uint32 */
        l_y1_dest = l_y0_dest + l_img_comp_dest->h;

        /*if (i == 0) {
        fprintf(stdout, "DEST: l_x0_dest=%d, l_x1_dest=%d, l_y0_dest=%d, l_y1_dest=%d (%d)\n",
                        l_x0_dest, l_x1_dest, l_y0_dest, l_y1_dest, l_img_comp_dest->factor );
        }*/

        /*-----*/
        /* Compute the area (l_offset_x0_src, l_offset_y0_src, l_offset_x1_src, l_offset_y1_src)
         * of the input buffer (decoded tile component) which will be move
         * in the output buffer. Compute the area of the output buffer (l_start_x_dest,
         * l_start_y_dest, l_width_dest, l_height_dest)  which will be modified
         * by this input area.
         * */
        assert(l_res->x0 >= 0);
        assert(l_res->x1 >= 0);
        if (l_x0_dest < (OPJ_UINT32)l_res->x0) {
            l_start_x_dest = (OPJ_UINT32)l_res->x0 - l_x0_dest;
            l_offset_x0_src = 0;

            if (l_x1_dest >= (OPJ_UINT32)l_res->x1) {
                l_width_dest = l_width_src;
                l_offset_x1_src = 0;
            } else {
                l_width_dest = l_x1_dest - (OPJ_UINT32)l_res->x0 ;
                l_offset_x1_src = (OPJ_INT32)(l_width_src - l_width_dest);
            }
        } else {
            l_start_x_dest = 0U;
            l_offset_x0_src = (OPJ_INT32)l_x0_dest - l_res->x0;

            if (l_x1_dest >= (OPJ_UINT32)l_res->x1) {
                l_width_dest = l_width_src - (OPJ_UINT32)l_offset_x0_src;
                l_offset_x1_src = 0;
            } else {
                l_width_dest = l_img_comp_dest->w ;
                l_offset_x1_src = l_res->x1 - (OPJ_INT32)l_x1_dest;
            }
        }

        if (l_y0_dest < (OPJ_UINT32)l_res->y0) {
            l_start_y_dest = (OPJ_UINT32)l_res->y0 - l_y0_dest;
            l_offset_y0_src = 0;

            if (l_y1_dest >= (OPJ_UINT32)l_res->y1) {
                l_height_dest = l_height_src;
                l_offset_y1_src = 0;
            } else {
                l_height_dest = l_y1_dest - (OPJ_UINT32)l_res->y0 ;
                l_offset_y1_src = (OPJ_INT32)(l_height_src - l_height_dest);
            }
        } else {
            l_start_y_dest = 0U;
            l_offset_y0_src = (OPJ_INT32)l_y0_dest - l_res->y0;

            if (l_y1_dest >= (OPJ_UINT32)l_res->y1) {
                l_height_dest = l_height_src - (OPJ_UINT32)l_offset_y0_src;
                l_offset_y1_src = 0;
            } else {
                l_height_dest = l_img_comp_dest->h ;
                l_offset_y1_src = l_res->y1 - (OPJ_INT32)l_y1_dest;
            }
        }

        if ((l_offset_x0_src < 0) || (l_offset_y0_src < 0) || (l_offset_x1_src < 0) ||
                (l_offset_y1_src < 0)) {
            return OPJ_FALSE;
        }
        /* testcase 2977.pdf.asan.67.2198 */
        if ((OPJ_INT32)l_width_dest < 0 || (OPJ_INT32)l_height_dest < 0) {
            return OPJ_FALSE;
        }
        /*-----*/

        /* Compute the input buffer offset */
        l_start_offset_src = (OPJ_SIZE_T)l_offset_x0_src + (OPJ_SIZE_T)l_offset_y0_src
                             * (OPJ_SIZE_T)l_width_src;
        l_line_offset_src  = (OPJ_SIZE_T)l_offset_x1_src + (OPJ_SIZE_T)l_offset_x0_src;
        l_end_offset_src   = (OPJ_SIZE_T)l_offset_y1_src * (OPJ_SIZE_T)l_width_src -
                             (OPJ_SIZE_T)l_offset_x0_src;

        /* Compute the output buffer offset */
        l_start_offset_dest = (OPJ_SIZE_T)l_start_x_dest + (OPJ_SIZE_T)l_start_y_dest
                              * (OPJ_SIZE_T)l_img_comp_dest->w;
        l_line_offset_dest  = (OPJ_SIZE_T)l_img_comp_dest->w - (OPJ_SIZE_T)l_width_dest;

        /* Move the output buffer to the first place where we will write*/
        l_dest_ptr = l_img_comp_dest->data + l_start_offset_dest;

        /*if (i == 0) {
                fprintf(stdout, "COMPO[%d]:\n",i);
                fprintf(stdout, "SRC: l_start_x_src=%d, l_start_y_src=%d, l_width_src=%d, l_height_src=%d\n"
                                "\t tile offset:%d, %d, %d, %d\n"
                                "\t buffer offset: %d; %d, %d\n",
                                l_res->x0, l_res->y0, l_width_src, l_height_src,
                                l_offset_x0_src, l_offset_y0_src, l_offset_x1_src, l_offset_y1_src,
                                l_start_offset_src, l_line_offset_src, l_end_offset_src);

                fprintf(stdout, "DEST: l_start_x_dest=%d, l_start_y_dest=%d, l_width_dest=%d, l_height_dest=%d\n"
                                "\t start offset: %d, line offset= %d\n",
                                l_start_x_dest, l_start_y_dest, l_width_dest, l_height_dest, l_start_offset_dest, l_line_offset_dest);
        }*/

        switch (l_size_comp) {
        case 1: {
            OPJ_CHAR * l_src_ptr = (OPJ_CHAR*) p_data;
            l_src_ptr += l_start_offset_src; /* Move to the first place where we will read*/

            if (l_img_comp_src->sgnd) {
                for (j = 0 ; j < l_height_dest ; ++j) {
                    for (k = 0 ; k < l_width_dest ; ++k) {
                        *(l_dest_ptr++) = (OPJ_INT32)(*
                                                      (l_src_ptr++));  /* Copy only the data needed for the output image */
                    }

                    l_dest_ptr +=
                        l_line_offset_dest; /* Move to the next place where we will write */
                    l_src_ptr += l_line_offset_src ; /* Move to the next place where we will read */
                }
            } else {
                for (j = 0 ; j < l_height_dest ; ++j) {
                    for (k = 0 ; k < l_width_dest ; ++k) {
                        *(l_dest_ptr++) = (OPJ_INT32)((*(l_src_ptr++)) & 0xff);
                    }

                    l_dest_ptr += l_line_offset_dest;
                    l_src_ptr += l_line_offset_src;
                }
            }

            l_src_ptr +=
                l_end_offset_src; /* Move to the end of this component-part of the input buffer */
            p_data = (OPJ_BYTE*)
                     l_src_ptr; /* Keep the current position for the next component-part */
        }
        break;
        case 2: {
            OPJ_INT16 * l_src_ptr = (OPJ_INT16 *) p_data;
            l_src_ptr += l_start_offset_src;

            if (l_img_comp_src->sgnd) {
                for (j = 0; j < l_height_dest; ++j) {
                    for (k = 0; k < l_width_dest; ++k) {
                        OPJ_INT16 val;
                        memcpy(&val, l_src_ptr, sizeof(val));
                        l_src_ptr ++;
                        *(l_dest_ptr++) = val;
                    }

                    l_dest_ptr += l_line_offset_dest;
                    l_src_ptr += l_line_offset_src ;
                }
            } else {
                for (j = 0; j < l_height_dest; ++j) {
                    for (k = 0; k < l_width_dest; ++k) {
                        OPJ_INT16 val;
                        memcpy(&val, l_src_ptr, sizeof(val));
                        l_src_ptr ++;
                        *(l_dest_ptr++) = val & 0xffff;
                    }

                    l_dest_ptr += l_line_offset_dest;
                    l_src_ptr += l_line_offset_src ;
                }
            }

            l_src_ptr += l_end_offset_src;
            p_data = (OPJ_BYTE*) l_src_ptr;
        }
        break;
        case 4: {
            OPJ_INT32 * l_src_ptr = (OPJ_INT32 *) p_data;
            l_src_ptr += l_start_offset_src;

            for (j = 0; j < l_height_dest; ++j) {
                memcpy(l_dest_ptr, l_src_ptr, l_width_dest * sizeof(OPJ_INT32));
                l_dest_ptr += l_width_dest + l_line_offset_dest;
                l_src_ptr += l_width_dest + l_line_offset_src ;
            }

            l_src_ptr += l_end_offset_src;
            p_data = (OPJ_BYTE*) l_src_ptr;
        }
        break;
        }

        ++l_img_comp_dest;
        ++l_img_comp_src;
        ++l_tilec;
    }

    return OPJ_TRUE;
}