xf86ScreenInit(ScreenPtr pScreen, int argc, char **argv)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);

    pScrn->pScreen = pScreen;
    return pScrn->ScreenInit (pScreen, argc, argv);
}