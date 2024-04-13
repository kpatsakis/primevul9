static int fits_nan_32 (unsigned char *v)

{register unsigned long k;

 k = (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
 k &= 0x7fffffff;  /* Dont care about the sign bit */

 /* See NOST Definition of the Flexible Image Transport System (FITS), */
 /* Appendix F, IEEE special formats. */
 return (   ((k >= 0x7f7fffff) && (k <= 0x7fffffff))
         || ((k >= 0x00000001) && (k <= 0x00800000)));
}