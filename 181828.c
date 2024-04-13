copyutf8(struct ly_ctx *ctx, char *dst, const char *src)
{
    uint32_t value;

    /* unicode characters */
    if (!(src[0] & 0x80)) {
        /* one byte character */
        if (src[0] < 0x20 &&
                src[0] != 0x09 &&
                src[0] != 0x0a &&
                src[0] != 0x0d) {
            LOGVAL(ctx, LYE_XML_INCHAR, LY_VLOG_NONE, NULL, src);
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid UTF-8 value 0x%02x", src[0]);
            return 0;
        }

        dst[0] = src[0];
        return 1;
    } else if (!(src[0] & 0x20)) {
        /* two bytes character */
        dst[0] = src[0];
        dst[1] = src[1];
        return 2;
    } else if (!(src[0] & 0x10)) {
        /* three bytes character */
        value = ((uint32_t)(src[0] & 0xf) << 12) | ((uint32_t)(src[1] & 0x3f) << 6) | (src[2] & 0x3f);
        if (((value & 0xf800) == 0xd800) ||
                (value >= 0xfdd0 && value <= 0xfdef) ||
                (value & 0xffe) == 0xffe) {
            /* exclude surrogate blocks %xD800-DFFF */
            /* exclude noncharacters %xFDD0-FDEF */
            /* exclude noncharacters %xFFFE-FFFF */
            LOGVAL(ctx, LYE_XML_INCHAR, LY_VLOG_NONE, NULL, src);
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid UTF-8 value 0x%08x", value);
            return 0;
        }

        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        return 3;
    } else if (!(src[0] & 0x08)) {
        /* four bytes character */
        value = ((uint32_t)(src[0] & 0x7) << 18) | ((uint32_t)(src[1] & 0x3f) << 12) | ((uint32_t)(src[2] & 0x3f) << 6) | (src[3] & 0x3f);
        if ((value & 0xffe) == 0xffe) {
            /* exclude noncharacters %x1FFFE-1FFFF, %x2FFFE-2FFFF, %x3FFFE-3FFFF, %x4FFFE-4FFFF,
             * %x5FFFE-5FFFF, %x6FFFE-6FFFF, %x7FFFE-7FFFF, %x8FFFE-8FFFF, %x9FFFE-9FFFF, %xAFFFE-AFFFF,
             * %xBFFFE-BFFFF, %xCFFFE-CFFFF, %xDFFFE-DFFFF, %xEFFFE-EFFFF, %xFFFFE-FFFFF, %x10FFFE-10FFFF */
            LOGVAL(ctx, LYE_XML_INCHAR, LY_VLOG_NONE, NULL, src);
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid UTF-8 value 0x%08x", value);
            return 0;
        }
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        return 4;
    } else {
        LOGVAL(ctx, LYE_XML_INCHAR, LY_VLOG_NONE, NULL, src);
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid UTF-8 leading byte 0x%02x", src[0]);
        return 0;
    }
}