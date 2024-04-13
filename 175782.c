set_cinfo_polarity(gx_device * dev, gx_color_polarity_t new_polarity)
{
#ifdef DEBUG
    /* sanity check */
    if (new_polarity == GX_CINFO_POLARITY_UNKNOWN) {
        dmprintf(dev->memory, "set_cinfo_polarity: illegal operand\n");
        return;
    }
#endif
    /*
     * The meory devices assume that single color devices are gray.
     * This may not be true if SeparationOrder is specified.  Thus only
     * change the value if the current value is unknown.
     */
    if (dev->color_info.polarity == GX_CINFO_POLARITY_UNKNOWN)
        dev->color_info.polarity = new_polarity;
}