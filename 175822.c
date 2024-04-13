set_linear_color_bits_mask_shift(gx_device * dev)
{
    int i;
    byte gray_index = dev->color_info.gray_index;
    gx_color_value max_gray = dev->color_info.max_gray;
    gx_color_value max_color = dev->color_info.max_color;
    int num_components = dev->color_info.num_components;

#define comp_bits (dev->color_info.comp_bits)
#define comp_mask (dev->color_info.comp_mask)
#define comp_shift (dev->color_info.comp_shift)
    comp_shift[num_components - 1] = 0;
    for ( i = num_components - 1 - 1; i >= 0; i-- ) {
        comp_shift[i] = comp_shift[i + 1] +
            ( i == gray_index ? ilog2(max_gray + 1) : ilog2(max_color + 1) );
    }
    for ( i = 0; i < num_components; i++ ) {
        comp_bits[i] = ( i == gray_index ?
                         ilog2(max_gray + 1) :
                         ilog2(max_color + 1) );
        comp_mask[i] = (((gx_color_index)1 << comp_bits[i]) - 1)
                                               << comp_shift[i];
    }
#undef comp_bits
#undef comp_mask
#undef comp_shift
}