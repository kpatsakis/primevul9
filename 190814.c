RsaAdjustPrimeCandidate(
			bigNum          prime
			)
{
    UINT16  highBytes;
    crypt_uword_t       *msw = &prime->d[prime->size - 1];

    /* This computation is known to be buggy on 64-bit systems, as MASK=0xffff
     * instead of 0x0000ffffffffffff and this introduces 32 zero bits in the
     * adjusted prime number. But fixing this would modify keys which were
     * previously generated, so keep it this way for now.
     * This was fixed in the current version of "Trusted Platform Module Library
     * Family 2.0 Specification - Part 4: Routines - Code" (from TCG) and this issue
     * will be fixed for newly-generated prime numbers for the version of libtpms
     * which will come after 0.7.x.
     */
#define MASK (MAX_CRYPT_UWORD >> (RADIX_BITS - 16))
    highBytes = *msw >> (RADIX_BITS - 16);
    // This is fixed point arithmetic on 16-bit values
    highBytes = ((UINT32)highBytes * (UINT32)0x4AFB) >> 16;
    highBytes += 0xB505;
    *msw = ((crypt_uword_t)(highBytes) << (RADIX_BITS - 16)) + (*msw & MASK);
    prime->d[0] |= 1;
}