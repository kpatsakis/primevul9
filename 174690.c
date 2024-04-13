xf86EnsureRANDR(ScreenPtr pScreen)
{
#ifdef RANDR
        if (!dixPrivateKeyRegistered(rrPrivKey) ||
            !rrGetScrPriv(pScreen))
            xf86RandRInit(pScreen);
#endif
}