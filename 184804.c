int main()
{
    gdImagePtr im;
    void *data;
    int size = 0;

    im = gdImageCreate(100, 100);
    gdTestAssert(im != NULL);

    data = gdImagePngPtr(im, &size);
    gdTestAssert(data == NULL);

    gdImageDestroy(im);

    return gdNumFailures();
}