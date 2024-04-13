int WavpackGetVersion (WavpackContext *wpc)
{
    if (wpc) {
#ifdef ENABLE_LEGACY
        if (wpc->stream3)
            return get_version3 (wpc);
#endif
        return wpc->version_five ? 5 : 4;
    }

    return 0;
}