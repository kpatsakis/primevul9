int main()
{
    gdImagePtr im;
    FILE *fp;
    void *data;
    int size = 0;

    fp = gdTestFileOpen2("png", "bug00381_2.gd");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromGd(fp);
    gdTestAssert(im != NULL);
    fclose(fp);

    data = gdImagePngPtr(im, &size);
    gdTestAssert(data == NULL);

    gdImageDestroy(im);

    return gdNumFailures();
}