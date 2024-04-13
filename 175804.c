gx_default_get_alpha_bits(gx_device * dev, graphics_object_type type)
{
    return (type == go_text ? dev->color_info.anti_alias.text_bits :
            dev->color_info.anti_alias.graphics_bits);
}