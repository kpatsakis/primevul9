static void *store_samples (void *dst, int32_t *src, int qmode, int bps, int count)
{
    if (qmode & QMODE_BIG_ENDIAN) {
        if ((qmode & QMODE_UNSIGNED_WORDS) || (bps == 1 && !(qmode & QMODE_SIGNED_BYTES)))
            return store_big_endian_unsigned_samples (dst, src, bps, count);
        else
            return store_big_endian_signed_samples (dst, src, bps, count);
    }
    else if ((qmode & QMODE_UNSIGNED_WORDS) || (bps == 1 && !(qmode & QMODE_SIGNED_BYTES)))
        return store_little_endian_unsigned_samples (dst, src, bps, count);
    else
        return store_little_endian_signed_samples (dst, src, bps, count);
}