jbig2_image_compose_opt_OR(const uint8_t *s, uint8_t *d, int early, int late, uint8_t mask, uint8_t rightmask, uint32_t bytewidth, uint32_t h, uint32_t shift, uint32_t dstride, uint32_t sstride)
{
    if (early || late)
        template_image_compose_opt(s, d, early, late, mask, rightmask, bytewidth, h, shift, dstride, sstride, JBIG2_COMPOSE_OR);
    else
        template_image_compose_opt(s, d, 0, 0, mask, rightmask, bytewidth, h, shift, dstride, sstride, JBIG2_COMPOSE_OR);
}