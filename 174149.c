int main()
{
    gdImagePtr im;
    FILE *fp;

    fp = gdTestFileOpen2("gif", "php_bug_75571.gif");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromGif(fp);
    gdTestAssert(im == NULL);
    fclose(fp);

    return gdNumFailures();
}