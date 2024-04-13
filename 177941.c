FloydSteinbergDitheringG(gx_device_bjc_printer *dev,
                         byte *row, byte *dithered, uint width,
                         uint raster, bool limit_extr)
{
    byte byteG=0, bitmask = 0x80; /* first bit */
    int i;
    int error = 0, delta;
    int err_corr;
    int *err_vect;

    if (dev->FloydSteinbergDirectionForward) {
        /* First  point */
        err_vect = dev->FloydSteinbergErrorsG + 1;

        for( i=width; i>0; i--, row++, err_vect++) { /* i, sample, error */
            err_corr = dev->bjc_gamma_tableK[255-(*row)] + dev->FloydSteinbergG;
            if(err_corr > 4080 && limit_extr) err_corr = 4080;
            error += err_corr + *(err_vect+1);     /* the error in 1/16 */

            if(error > dev->bjc_treshold[bjc_rand(dev)])  {
                error -=  4080;
                byteG |=  bitmask;
            }

            *(err_vect+1) = (error + 8) >> 4;
            delta = error << 1;                                 /* 2 err */
            error += delta;                                     /* 3/16  */
            *(err_vect-1) += (error + 8) >> 4;
            error += delta;                                     /* 5/16  */
            *err_vect += (error + 8) >> 4;
            error += delta + 8;                                 /* 7/16  */
            error >>= 4;

            if (bitmask == 0x01) {
                *dithered = byteG;
                bitmask = 0x80;
                byteG = 0;
                dithered++;
            }
            else if (i == 1) {
                *dithered = byteG;
            }
            else bitmask >>= 1;
        }
        dev->FloydSteinbergDirectionForward=false;
    }
    else {
        row += width - 1;                   /* point to the end of the row */
        dithered += raster - 1;
        bitmask = 1 << ((raster << 3 ) - width) ;
        err_vect = dev->FloydSteinbergErrorsG + width + 1;

        for( i=width; i>0; i--, row--, err_vect--) {
            err_corr = dev->bjc_gamma_tableK[255-(*row)] + dev->FloydSteinbergG;
            if(err_corr > 4080 && limit_extr) err_corr = 4080;

            error += err_corr + *(err_vect - 1);

            if(error > dev->bjc_treshold[bjc_rand(dev)])  {
                error -=  4080;
                byteG |=  bitmask;
            }

            *(err_vect-1) = (error + 8) >> 4;                  /* 1/16 */
            delta = error << 1;                                 /* 2 err */
            error += delta;
            *(err_vect+1) += (error +8)  >> 4;                   /* 3/16  */
            error += delta;
            *err_vect += (error + 8)  >> 4;                      /* 5/16  */
            error += delta + 8;                                  /* 7/16  */
            error >>= 4;

            if (bitmask == 0x80) {
                *dithered = byteG;
                bitmask = 0x01;
                byteG = 0;
                dithered--;
            } else if(i==1) {
                *dithered = byteG;
            }
            else bitmask <<= 1;
        }
        dev->FloydSteinbergDirectionForward=true;
    }
}