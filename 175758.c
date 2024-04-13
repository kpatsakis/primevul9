int gx_copy_device_procs(gx_device *dest, gx_device *src, gx_device *prototype)
{
    set_dev_proc(dest, open_device, dev_proc(prototype, open_device));
    set_dev_proc(dest, get_initial_matrix, dev_proc(prototype, get_initial_matrix));
    set_dev_proc(dest, sync_output, dev_proc(prototype, sync_output));
    set_dev_proc(dest, output_page, dev_proc(prototype, output_page));
    set_dev_proc(dest, close_device, dev_proc(prototype, close_device));
    set_dev_proc(dest, map_rgb_color, dev_proc(prototype, map_rgb_color));
    set_dev_proc(dest, map_color_rgb, dev_proc(prototype, map_color_rgb));
    set_dev_proc(dest, fill_rectangle, dev_proc(prototype, fill_rectangle));
    set_dev_proc(dest, tile_rectangle, dev_proc(prototype, tile_rectangle));
    set_dev_proc(dest, copy_mono, dev_proc(prototype, copy_mono));
    set_dev_proc(dest, copy_color, dev_proc(prototype, copy_color));
    set_dev_proc(dest, obsolete_draw_line, dev_proc(prototype, obsolete_draw_line));
    set_dev_proc(dest, get_bits, dev_proc(prototype, get_bits));
    set_dev_proc(dest, get_params, dev_proc(prototype, get_params));
    set_dev_proc(dest, put_params, dev_proc(prototype, put_params));
    set_dev_proc(dest, map_cmyk_color, dev_proc(prototype, map_cmyk_color));
    set_dev_proc(dest, get_xfont_procs, dev_proc(prototype, get_xfont_procs));
    set_dev_proc(dest, get_xfont_device, dev_proc(prototype, get_xfont_device));
    set_dev_proc(dest, map_rgb_alpha_color, dev_proc(prototype, map_rgb_alpha_color));
    set_dev_proc(dest, get_page_device, dev_proc(prototype, get_page_device));
    set_dev_proc(dest, get_alpha_bits, dev_proc(prototype, get_alpha_bits));
    set_dev_proc(dest, copy_alpha, dev_proc(prototype, copy_alpha));
    set_dev_proc(dest, get_band, dev_proc(prototype, get_band));
    set_dev_proc(dest, copy_rop, dev_proc(prototype, copy_rop));
    set_dev_proc(dest, fill_path, dev_proc(prototype, fill_path));
    set_dev_proc(dest, stroke_path, dev_proc(prototype, stroke_path));
    set_dev_proc(dest, fill_trapezoid, dev_proc(prototype, fill_trapezoid));
    set_dev_proc(dest, fill_parallelogram, dev_proc(prototype, fill_parallelogram));
    set_dev_proc(dest, fill_triangle, dev_proc(prototype, fill_triangle));
    set_dev_proc(dest, draw_thin_line, dev_proc(prototype, draw_thin_line));
    set_dev_proc(dest, begin_image, dev_proc(prototype, begin_image));
    set_dev_proc(dest, image_data, dev_proc(prototype, image_data));
    set_dev_proc(dest, end_image, dev_proc(prototype, end_image));
    set_dev_proc(dest, strip_tile_rectangle, dev_proc(prototype, strip_tile_rectangle));
    set_dev_proc(dest, strip_copy_rop, dev_proc(prototype, strip_copy_rop));
    set_dev_proc(dest, get_clipping_box, dev_proc(prototype, get_clipping_box));
    set_dev_proc(dest, begin_typed_image, dev_proc(prototype, begin_typed_image));
    set_dev_proc(dest, get_bits_rectangle, dev_proc(prototype, get_bits_rectangle));
    set_dev_proc(dest, map_color_rgb_alpha, dev_proc(prototype, map_color_rgb_alpha));
    set_dev_proc(dest, create_compositor, dev_proc(prototype, create_compositor));
    set_dev_proc(dest, get_hardware_params, dev_proc(prototype, get_hardware_params));
    set_dev_proc(dest, text_begin, dev_proc(prototype, text_begin));
    set_dev_proc(dest, finish_copydevice, dev_proc(prototype, finish_copydevice));
    set_dev_proc(dest, discard_transparency_layer, dev_proc(prototype, discard_transparency_layer));
    set_dev_proc(dest, get_color_mapping_procs, dev_proc(prototype, get_color_mapping_procs));
    set_dev_proc(dest, get_color_comp_index, dev_proc(prototype, get_color_comp_index));
    set_dev_proc(dest, encode_color, dev_proc(prototype, encode_color));
    set_dev_proc(dest, decode_color, dev_proc(prototype, decode_color));
    set_dev_proc(dest, pattern_manage, dev_proc(prototype, pattern_manage));
    set_dev_proc(dest, fill_rectangle_hl_color, dev_proc(prototype, fill_rectangle_hl_color));
    set_dev_proc(dest, include_color_space, dev_proc(prototype, include_color_space));
    set_dev_proc(dest, fill_linear_color_scanline, dev_proc(prototype, fill_linear_color_scanline));
    set_dev_proc(dest, fill_linear_color_trapezoid, dev_proc(prototype, fill_linear_color_trapezoid));
    set_dev_proc(dest, fill_linear_color_triangle, dev_proc(prototype, fill_linear_color_triangle));
    set_dev_proc(dest, update_spot_equivalent_colors, dev_proc(prototype, update_spot_equivalent_colors));
    set_dev_proc(dest, ret_devn_params, dev_proc(prototype, ret_devn_params));
    set_dev_proc(dest, fillpage, dev_proc(prototype, fillpage));
    set_dev_proc(dest, push_transparency_state, dev_proc(prototype, push_transparency_state));
    set_dev_proc(dest, pop_transparency_state, dev_proc(prototype, pop_transparency_state));
    set_dev_proc(dest, dev_spec_op, dev_proc(prototype, dev_spec_op));
    set_dev_proc(dest, get_profile, dev_proc(prototype, get_profile));
    set_dev_proc(dest, strip_copy_rop2, dev_proc(prototype, strip_copy_rop2));
    set_dev_proc(dest, strip_tile_rect_devn, dev_proc(prototype, strip_tile_rect_devn));
    set_dev_proc(dest, process_page, dev_proc(prototype, process_page));

    /*
     * We absolutely must set the 'set_graphics_type_tag' to the default subclass one
     * even if the subclassed device is using the default. This is because the
     * default implementation sets a flag in the device structure, and if we
     * copy the default method, we'lll end up setting the flag in the subclassing device
     * instead of the subclassed device!
     */
    set_dev_proc(dest, set_graphics_type_tag, dev_proc(prototype, set_graphics_type_tag));

    /* These are the routines whose existence is checked against the default at
     * some point in the code. The code path differs when the device implements a
     * method other than the default, so the subclassing device needs to ensure that
     * if the subclassed device has one of these methods set to the default, we
     * do not overwrite the default method.
     */
    if (dev_proc(src, fill_mask) != gx_default_fill_mask)
        set_dev_proc(dest, fill_mask, dev_proc(prototype, fill_mask));
    if (dev_proc(src, begin_transparency_group) != gx_default_begin_transparency_group)
        set_dev_proc(dest, begin_transparency_group, dev_proc(prototype, begin_transparency_group));
    if (dev_proc(src, end_transparency_group) != gx_default_end_transparency_group)
        set_dev_proc(dest, end_transparency_group, dev_proc(prototype, end_transparency_group));
    if (dev_proc(src, put_image) != gx_default_put_image)
        set_dev_proc(dest, put_image, dev_proc(prototype, put_image));
    if (dev_proc(src, copy_planes) != gx_default_copy_planes)
        set_dev_proc(dest, copy_planes, dev_proc(prototype, copy_planes));
    if (dev_proc(src, copy_alpha_hl_color) != gx_default_no_copy_alpha_hl_color)
        set_dev_proc(dest, copy_alpha_hl_color, dev_proc(prototype, copy_alpha_hl_color));
    return 0;
}