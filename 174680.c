xf86GetPixFormat(ScrnInfoPtr pScrn, int depth)
{
    int i;

    for (i = 0; i < numFormats; i++)
        if (formats[i].depth == depth)
            break;
    if (i != numFormats)
        return &formats[i];
    else if (!formatsDone) {
        /* Check for screen-specified formats */
        for (i = 0; i < pScrn->numFormats; i++)
            if (pScrn->formats[i].depth == depth)
                break;
        if (i != pScrn->numFormats)
            return &pScrn->formats[i];
    }
    return NULL;
}