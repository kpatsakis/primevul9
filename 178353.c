template_image_compose_opt(const uint8_t * JBIG2_RESTRICT ss, uint8_t * JBIG2_RESTRICT dd, int early, int late, uint8_t leftmask, uint8_t rightmask, uint32_t bytewidth_, uint32_t h, uint32_t shift, uint32_t dstride, uint32_t sstride, Jbig2ComposeOp op)
{
    int i;
    uint32_t j;
    int bytewidth = (int)bytewidth_;

    if (bytewidth == 1) {
        for (j = 0; j < h; j++) {
            /* Only 1 byte! */
            uint8_t v = (((early ? 0 : ss[0]<<8) | (late ? 0 : ss[1]))>>shift);
            if (op == JBIG2_COMPOSE_OR)
                *dd |= v & leftmask;
            else if (op == JBIG2_COMPOSE_AND)
                *dd &= (v & leftmask) | ~leftmask;
            else if (op == JBIG2_COMPOSE_XOR)
                *dd ^= v & leftmask;
            else if (op == JBIG2_COMPOSE_XNOR)
                *dd ^= (~v) & leftmask;
            else /* Replace */
                *dd = (v & leftmask) | (*dd & ~leftmask);
            dd += dstride;
            ss += sstride;
        }
        return;
    }
    bytewidth -= 2;
    if (shift == 0) {
        ss++;
        for (j = 0; j < h; j++) {
            /* Left byte */
            const uint8_t * JBIG2_RESTRICT s = ss;
            uint8_t * JBIG2_RESTRICT d = dd;
            if (op == JBIG2_COMPOSE_OR)
                *d++ |= *s++ & leftmask;
            else if (op == JBIG2_COMPOSE_AND)
                *d++ &= (*s++ & leftmask) | ~leftmask;
            else if (op == JBIG2_COMPOSE_XOR)
                *d++ ^= *s++ & leftmask;
            else if (op == JBIG2_COMPOSE_XNOR)
                *d++ ^= (~*s++) & leftmask;
            else /* Replace */
                *d = (*s++ & leftmask) | (*d & ~leftmask), d++;
            /* Central run */
            for (i = bytewidth; i != 0; i--) {
                if (op == JBIG2_COMPOSE_OR)
                    *d++ |= *s++;
                else if (op == JBIG2_COMPOSE_AND)
                    *d++ &= *s++;
                else if (op == JBIG2_COMPOSE_XOR)
                    *d++ ^= *s++;
                else if (op == JBIG2_COMPOSE_XNOR)
                    *d++ ^= ~*s++;
                else /* Replace */
                    *d++ = *s++;
            }
            /* Right byte */
            if (op == JBIG2_COMPOSE_OR)
                *d |= *s & rightmask;
            else if (op == JBIG2_COMPOSE_AND)
                *d &= (*s & rightmask) | ~rightmask;
            else if (op == JBIG2_COMPOSE_XOR)
                *d ^= *s & rightmask;
            else if (op == JBIG2_COMPOSE_XNOR)
                *d ^= (~*s) & rightmask;
            else /* Replace */
                *d = (*s & rightmask) | (*d & ~rightmask);
            dd += dstride;
            ss += sstride;
        }
    } else {
        for (j = 0; j < h; j++) {
            /* Left byte */
            const uint8_t * JBIG2_RESTRICT s = ss;
            uint8_t * JBIG2_RESTRICT d = dd;
            uint8_t s0, s1, v;
            s0 = early ? 0 : *s;
            s++;
            s1 = *s++;
            v = ((s0<<8) | s1)>>shift;
            if (op == JBIG2_COMPOSE_OR)
                *d++ |= v & leftmask;
            else if (op == JBIG2_COMPOSE_AND)
                *d++ &= (v & leftmask) | ~leftmask;
            else if (op == JBIG2_COMPOSE_XOR)
                *d++ ^= v & leftmask;
            else if (op == JBIG2_COMPOSE_XNOR)
                *d++ ^= (~v) & leftmask;
            else /* Replace */
                *d = (v & leftmask) | (*d & ~leftmask), d++;
            /* Central run */
            for (i = bytewidth; i > 0; i--) {
                s0 = s1; s1 = *s++;
                v = ((s0<<8) | s1)>>shift;
                if (op == JBIG2_COMPOSE_OR)
                    *d++ |= v;
                else if (op == JBIG2_COMPOSE_AND)
                    *d++ &= v;
                else if (op == JBIG2_COMPOSE_XOR)
                    *d++ ^= v;
                else if (op == JBIG2_COMPOSE_XNOR)
                    *d++ ^= ~v;
                else /* Replace */
                    *d++ = v;
            }
            /* Right byte */
            s0 = s1; s1 = (late ? 0 : *s);
            v = (((s0<<8) | s1)>>shift);
            if (op == JBIG2_COMPOSE_OR)
                *d |= v & rightmask;
            else if (op == JBIG2_COMPOSE_AND)
                *d &= (v & rightmask) | ~rightmask;
            else if (op == JBIG2_COMPOSE_XOR)
                *d ^= v & rightmask;
            else if (op == JBIG2_COMPOSE_XNOR)
                *d ^= ~v & rightmask;
            else /* Replace */
                *d = (v & rightmask) | (*d & ~rightmask);
            dd += dstride;
            ss += sstride;
        }
    }
}