int bmpTest(void)
{
	int align, width=35, height=39, format;

	for(align=1; align<=8; align*=2)
	{
		for(format=0; format<TJ_NUMPF; format++)
		{
			printf("%s Top-Down BMP (row alignment = %d bytes)  ...  ",
				pixFormatStr[format], align);
			if(doBmpTest("bmp", width, align, height, format, 0)==-1)
				return -1;
			printf("OK.\n");

			printf("%s Top-Down PPM (row alignment = %d bytes)  ...  ",
				pixFormatStr[format], align);
			if(doBmpTest("ppm", width, align, height, format, TJFLAG_BOTTOMUP)==-1)
				return -1;
			printf("OK.\n");

			printf("%s Bottom-Up BMP (row alignment = %d bytes)  ...  ",
				pixFormatStr[format], align);
			if(doBmpTest("bmp", width, align, height, format, 0)==-1)
				return -1;
			printf("OK.\n");

			printf("%s Bottom-Up PPM (row alignment = %d bytes)  ...  ",
				pixFormatStr[format], align);
			if(doBmpTest("ppm", width, align, height, format, TJFLAG_BOTTOMUP)==-1)
				return -1;
			printf("OK.\n");
		}
	}

	return 0;
}