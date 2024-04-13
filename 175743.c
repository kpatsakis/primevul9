gx_device_fill_in_procs(register gx_device * dev)
{
    gx_device_set_procs(dev);
    fill_dev_proc(dev, open_device, gx_default_open_device);
    fill_dev_proc(dev, get_initial_matrix, gx_default_get_initial_matrix);
    fill_dev_proc(dev, sync_output, gx_default_sync_output);
    fill_dev_proc(dev, output_page, gx_default_output_page);
    fill_dev_proc(dev, close_device, gx_default_close_device);
    /* see below for map_rgb_color */
    fill_dev_proc(dev, map_color_rgb, gx_default_map_color_rgb);
    /* NOT fill_rectangle */
    fill_dev_proc(dev, tile_rectangle, gx_default_tile_rectangle);
    fill_dev_proc(dev, copy_mono, gx_default_copy_mono);
    fill_dev_proc(dev, copy_color, gx_default_copy_color);
    fill_dev_proc(dev, obsolete_draw_line, gx_default_draw_line);
    fill_dev_proc(dev, get_bits, gx_default_get_bits);
    fill_dev_proc(dev, get_params, gx_default_get_params);
    fill_dev_proc(dev, put_params, gx_default_put_params);
    /* see below for map_cmyk_color */
    fill_dev_proc(dev, get_xfont_procs, gx_default_get_xfont_procs);
    fill_dev_proc(dev, get_xfont_device, gx_default_get_xfont_device);
    fill_dev_proc(dev, map_rgb_alpha_color, gx_default_map_rgb_alpha_color);
    fill_dev_proc(dev, get_page_device, gx_default_get_page_device);
    fill_dev_proc(dev, get_alpha_bits, gx_default_get_alpha_bits);
    fill_dev_proc(dev, copy_alpha, gx_default_copy_alpha);
    fill_dev_proc(dev, get_band, gx_default_get_band);
    fill_dev_proc(dev, copy_rop, gx_default_copy_rop);
    fill_dev_proc(dev, fill_path, gx_default_fill_path);
    fill_dev_proc(dev, stroke_path, gx_default_stroke_path);
    fill_dev_proc(dev, fill_mask, gx_default_fill_mask);
    fill_dev_proc(dev, fill_trapezoid, gx_default_fill_trapezoid);
    fill_dev_proc(dev, fill_parallelogram, gx_default_fill_parallelogram);
    fill_dev_proc(dev, fill_triangle, gx_default_fill_triangle);
    fill_dev_proc(dev, draw_thin_line, gx_default_draw_thin_line);
    fill_dev_proc(dev, begin_image, gx_default_begin_image);
    /*
     * We always replace get_alpha_bits, image_data, and end_image with the
     * new procedures, and, if in a DEBUG configuration, print a warning if
     * the definitions aren't the default ones.
     */
#ifdef DEBUG
#  define CHECK_NON_DEFAULT(proc, default, procname)\
    BEGIN\
        if ( dev_proc(dev, proc) != NULL && dev_proc(dev, proc) != default )\
            dmprintf2(dev->memory, "**** Warning: device %s implements obsolete procedure %s\n",\
                     dev->dname, procname);\
    END
#else
#  define CHECK_NON_DEFAULT(proc, default, procname)\
    DO_NOTHING
#endif
    CHECK_NON_DEFAULT(get_alpha_bits, gx_default_get_alpha_bits,
                      "get_alpha_bits");
    set_dev_proc(dev, get_alpha_bits, gx_default_get_alpha_bits);
    CHECK_NON_DEFAULT(image_data, gx_default_image_data, "image_data");
    set_dev_proc(dev, image_data, gx_default_image_data);
    CHECK_NON_DEFAULT(end_image, gx_default_end_image, "end_image");
    set_dev_proc(dev, end_image, gx_default_end_image);
#undef CHECK_NON_DEFAULT
    fill_dev_proc(dev, strip_tile_rectangle, gx_default_strip_tile_rectangle);
    fill_dev_proc(dev, strip_copy_rop, gx_default_strip_copy_rop);
    fill_dev_proc(dev, strip_copy_rop2, gx_default_strip_copy_rop2);
    fill_dev_proc(dev, strip_tile_rect_devn, gx_default_strip_tile_rect_devn);
    fill_dev_proc(dev, get_clipping_box, gx_default_get_clipping_box);
    fill_dev_proc(dev, begin_typed_image, gx_default_begin_typed_image);
    fill_dev_proc(dev, get_bits_rectangle, gx_default_get_bits_rectangle);
    fill_dev_proc(dev, map_color_rgb_alpha, gx_default_map_color_rgb_alpha);
    fill_dev_proc(dev, create_compositor, gx_default_create_compositor);
    fill_dev_proc(dev, get_hardware_params, gx_default_get_hardware_params);
    fill_dev_proc(dev, text_begin, gx_default_text_begin);
    fill_dev_proc(dev, finish_copydevice, gx_default_finish_copydevice);

    set_dev_proc(dev, encode_color, get_encode_color(dev));
    if (dev->color_info.num_components == 3)
        set_dev_proc(dev, map_rgb_color, dev_proc(dev, encode_color));
    if (dev->color_info.num_components == 4)
        set_dev_proc(dev, map_cmyk_color, dev_proc(dev, encode_color));

    if (colors_are_separable_and_linear(&dev->color_info)) {
        fill_dev_proc(dev, encode_color, gx_default_encode_color);
        fill_dev_proc(dev, map_cmyk_color, gx_default_encode_color);
        fill_dev_proc(dev, map_rgb_color, gx_default_encode_color);
    } else {
        /* if it isn't set now punt */
        fill_dev_proc(dev, encode_color, gx_error_encode_color);
        fill_dev_proc(dev, map_cmyk_color, gx_error_encode_color);
        fill_dev_proc(dev, map_rgb_color, gx_error_encode_color);
    }

    /*
     * Fill in the color mapping procedures and the component index
     * assignment procedure if they have not been provided by the client.
     *
     * Because it is difficult to provide default encoding procedures
     * that handle level inversion, this code needs to check both
     * the number of components and the polarity of color model.
     */
    switch (dev->color_info.num_components) {
    case 1:     /* DeviceGray or DeviceInvertGray */
        /*
         * If not gray then the device must provide the color
         * mapping procs.
         */
        if (dev->color_info.polarity == GX_CINFO_POLARITY_ADDITIVE) {
            fill_dev_proc( dev,
                       get_color_mapping_procs,
                       gx_default_DevGray_get_color_mapping_procs );
        } else
            fill_dev_proc(dev, get_color_mapping_procs, gx_error_get_color_mapping_procs);
        fill_dev_proc( dev,
                       get_color_comp_index,
                       gx_default_DevGray_get_color_comp_index );
        break;

    case 3:
        if (dev->color_info.polarity == GX_CINFO_POLARITY_ADDITIVE) {
            fill_dev_proc( dev,
                       get_color_mapping_procs,
                       gx_default_DevRGB_get_color_mapping_procs );
            fill_dev_proc( dev,
                       get_color_comp_index,
                       gx_default_DevRGB_get_color_comp_index );
        } else {
            fill_dev_proc(dev, get_color_mapping_procs, gx_error_get_color_mapping_procs);
            fill_dev_proc(dev, get_color_comp_index, gx_error_get_color_comp_index);
        }
        break;

    case 4:
        fill_dev_proc(dev, get_color_mapping_procs, gx_default_DevCMYK_get_color_mapping_procs);
        fill_dev_proc(dev, get_color_comp_index, gx_default_DevCMYK_get_color_comp_index);
        break;
    default:		/* Unknown color model - set error handlers */
        if (dev_proc(dev, get_color_mapping_procs) == NULL) {
            fill_dev_proc(dev, get_color_mapping_procs, gx_error_get_color_mapping_procs);
            fill_dev_proc(dev, get_color_comp_index, gx_error_get_color_comp_index);
        }
    }

    set_dev_proc(dev, decode_color, get_decode_color(dev));
    fill_dev_proc(dev, get_profile, gx_default_get_profile);
    fill_dev_proc(dev, set_graphics_type_tag, gx_default_set_graphics_type_tag);

    /*
     * If the device is known not to support overprint mode, indicate this now.
     * Note that we do not insist that a device be use a strict DeviceCMYK
     * encoding; any color model that is subtractive and supports the cyan,
     * magenta, yellow, and black color components will do. We defer a more
     * explicit check until this information is explicitly required.
     */
    if ( dev->color_info.opmode == GX_CINFO_OPMODE_UNKNOWN          &&
         (dev->color_info.num_components < 4                     ||
          dev->color_info.polarity == GX_CINFO_POLARITY_ADDITIVE ||
          dev->color_info.gray_index == GX_CINFO_COMP_NO_INDEX     )  )
        dev->color_info.opmode = GX_CINFO_OPMODE_NOT;

    fill_dev_proc(dev, fill_rectangle_hl_color, gx_default_fill_rectangle_hl_color);
    fill_dev_proc(dev, include_color_space, gx_default_include_color_space);
    fill_dev_proc(dev, fill_linear_color_scanline, gx_default_fill_linear_color_scanline);
    fill_dev_proc(dev, fill_linear_color_trapezoid, gx_default_fill_linear_color_trapezoid);
    fill_dev_proc(dev, fill_linear_color_triangle, gx_default_fill_linear_color_triangle);
    fill_dev_proc(dev, update_spot_equivalent_colors, gx_default_update_spot_equivalent_colors);
    fill_dev_proc(dev, ret_devn_params, gx_default_ret_devn_params);
    fill_dev_proc(dev, fillpage, gx_default_fillpage);
    fill_dev_proc(dev, copy_alpha_hl_color, gx_default_no_copy_alpha_hl_color);

    fill_dev_proc(dev, begin_transparency_group, gx_default_begin_transparency_group);
    fill_dev_proc(dev, end_transparency_group, gx_default_end_transparency_group);

    fill_dev_proc(dev, begin_transparency_mask, gx_default_begin_transparency_mask);
    fill_dev_proc(dev, end_transparency_mask, gx_default_end_transparency_mask);
    fill_dev_proc(dev, discard_transparency_layer, gx_default_discard_transparency_layer);

    fill_dev_proc(dev, pattern_manage, gx_default_pattern_manage);
    fill_dev_proc(dev, push_transparency_state, gx_default_push_transparency_state);
    fill_dev_proc(dev, pop_transparency_state, gx_default_pop_transparency_state);

    fill_dev_proc(dev, put_image, gx_default_put_image);

    fill_dev_proc(dev, dev_spec_op, gx_default_dev_spec_op);
    fill_dev_proc(dev, copy_planes, gx_default_copy_planes);
    fill_dev_proc(dev, process_page, gx_default_process_page);
}