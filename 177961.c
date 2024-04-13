void bjc_build_gamma_table(gx_device_bjc_printer *dev, float gamma, char color)
{ int i;
  int *table;

    switch(color) {
    case CMYK_C:
        table = dev->bjc_gamma_tableC;
        break;
    case CMYK_M:
        table = dev->bjc_gamma_tableM;
        break;
    case CMYK_Y:
        table = dev->bjc_gamma_tableY;
        break;
    case CMYK_K:
    default:
        table = dev->bjc_gamma_tableK;
        break;
    }

    if(gamma == 1.0) for (i = 0; i < 256; i++) table[i] = (255 - i) << 4;
    else for (i = 0; i < 256; i++) table[i] =
                  4080 - (int)(pow((double)i / 255.0, gamma) * 4080.0 + .5);
}