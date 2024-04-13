copy_gray_row (gint     *dest,
               guchar   *src,
               gint      width,
               gboolean  has_alpha)
{
        gint i;
        guchar *p;

        p = src;
        for (i = 0; i < width; i++) {
                int pr, pg, pb, pv;

                pr = *p++;
                pg = *p++;
                pb = *p++;

                if (has_alpha) {
                        int pa = *p++;

                        /* Premul alpha to simulate it */
                        if (pa > 0) {
                                pr = pr * pa / 255;
                                pg = pg * pa / 255;
                                pb = pb * pa / 255;
                        } else {
                                pr = pg = pb = 0;
                        }
                }

                /* Calculate value MAX(MAX(r,g),b) */
                pv = pr > pg ? pr : pg;
                pv = pv > pb ? pv : pb;

                *dest++ = pv;
        }
}