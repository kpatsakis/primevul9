void writeJPEG(unsigned char *jpegBuf, unsigned long jpegSize, char *filename)
{
	FILE *file=fopen(filename, "wb");
	if(!file || fwrite(jpegBuf, jpegSize, 1, file)!=1)
	{
		printf("ERROR: Could not write to %s.\n%s\n", filename, strerror(errno));
		bailout();
	}

	bailout:
	if(file) fclose(file);
}