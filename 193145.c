hufUncompress (const char compressed[],
	       int nCompressed,
	       unsigned short raw[],
	       int nRaw)
{
    //
    // need at least 20 bytes for header
    //
    if (nCompressed < 20 )
    {
	if (nRaw != 0)
	    notEnoughData();

	return;
    }

    int im = readUInt (compressed);
    int iM = readUInt (compressed + 4);
    // int tableLength = readUInt (compressed + 8);
    int nBits = readUInt (compressed + 12);

    if (im < 0 || im >= HUF_ENCSIZE || iM < 0 || iM >= HUF_ENCSIZE)
	invalidTableSize();

    const char *ptr = compressed + 20;

    uint64_t nBytes = (static_cast<uint64_t>(nBits)+7) / 8 ;

    if ( ptr + nBytes > compressed+nCompressed)
    {
        notEnoughData();
        return;
    }

    // 
    // Fast decoder needs at least 2x64-bits of compressed data, and
    // needs to be run-able on this platform. Otherwise, fall back
    // to the original decoder
    //

    if (FastHufDecoder::enabled() && nBits > 128)
    {
        FastHufDecoder fhd (ptr, nCompressed - (ptr - compressed), im, iM, iM);
        fhd.decode ((unsigned char*)ptr, nBits, raw, nRaw);
    }
    else
    {
        AutoArray <Int64, HUF_ENCSIZE> freq;
        AutoArray <HufDec, HUF_DECSIZE> hdec;

        hufClearDecTable (hdec);

        hufUnpackEncTable (&ptr,
                           nCompressed - (ptr - compressed),
                           im,
                           iM,
                           freq);

        try
        {
            if (nBits > 8 * (nCompressed - (ptr - compressed)))
                invalidNBits();

            hufBuildDecTable (freq, im, iM, hdec);
            hufDecode (freq, hdec, ptr, nBits, iM, nRaw, raw);
        }
        catch (...)
        {
            hufFreeDecTable (hdec);
            throw;
        }

        hufFreeDecTable (hdec);
    }
}