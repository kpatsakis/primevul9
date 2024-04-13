ImagingNewPrologue(const char *mode, int xsize, int ysize)
{
    return ImagingNewPrologueSubtype(
        mode, xsize, ysize, sizeof(struct ImagingMemoryInstance)
        );
}