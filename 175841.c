gx_default_set_graphics_type_tag(gx_device *dev, gs_graphics_type_tag_t graphics_type_tag)
{
    /* set the tag but carefully preserve GS_DEVICE_ENCODES_TAGS */
    dev->graphics_type_tag = (dev->graphics_type_tag & GS_DEVICE_ENCODES_TAGS) | graphics_type_tag;
}