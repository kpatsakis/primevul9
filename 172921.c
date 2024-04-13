void usage(char *progName)
{
	printf("\nUSAGE: %s [options]\n\n", progName);
	printf("Options:\n");
	printf("-yuv = test YUV encoding/decoding support\n");
	printf("-noyuvpad = do not pad each line of each Y, U, and V plane to the nearest\n");
	printf("            4-byte boundary\n");
	printf("-alloc = test automatic buffer allocation\n");
	printf("-bmp = tjLoadImage()/tjSaveImage() unit test\n\n");
	exit(1);
}