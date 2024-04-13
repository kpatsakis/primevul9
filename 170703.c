decode_row_cie(const gx_image_enum *penum, const byte *psrc, int spp, byte *pdes,
                byte *bufend, gs_range range_array[])
{
    byte *curr_pos = pdes;
    int k;
    float temp;

    while ( curr_pos < bufend ) {
        for ( k = 0; k < spp; k ++ ) {
            switch ( penum->map[k].decoding ) {
                case sd_none:
                    *curr_pos = *psrc;
                    break;
                case sd_lookup:
                    temp = penum->map[k].decode_lookup[(*psrc) >> 4]*255.0;
                    temp = rescale_input_color(range_array[k], temp);
                    temp = temp*255;
                    if (temp > 255) temp = 255;
                    if (temp < 0 ) temp = 0;
                    *curr_pos = (unsigned char) temp;
                    break;
                case sd_compute:
                    temp = penum->map[k].decode_base +
                        (*psrc) * penum->map[k].decode_factor;
                    temp = rescale_input_color(range_array[k], temp);
                    temp = temp*255;
                    if (temp > 255) temp = 255;
                    if (temp < 0 ) temp = 0;
                    *curr_pos = (unsigned char) temp;
                default:
                    break;
            }
            curr_pos++;
            psrc++;
        }
    }
}