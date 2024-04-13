JBIG2PatternDict::~JBIG2PatternDict()
{
    unsigned int i;

    for (i = 0; i < size; ++i) {
        delete bitmaps[i];
    }
    gfree(bitmaps);
}