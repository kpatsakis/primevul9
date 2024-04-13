TDStretch * TDStretch::newInstance()
{
    uint uExtensions;

    uExtensions = detectCPUextensions();

    // Check if MMX/SSE instruction set extensions supported by CPU

#ifdef SOUNDTOUCH_ALLOW_MMX
    // MMX routines available only with integer sample types
    if (uExtensions & SUPPORT_MMX)
    {
        return ::new TDStretchMMX;
    }
    else
#endif // SOUNDTOUCH_ALLOW_MMX


#ifdef SOUNDTOUCH_ALLOW_SSE
    if (uExtensions & SUPPORT_SSE)
    {
        // SSE support
        return ::new TDStretchSSE;
    }
    else
#endif // SOUNDTOUCH_ALLOW_SSE

    {
        // ISA optimizations not supported, use plain C version
        return ::new TDStretch;
    }
}