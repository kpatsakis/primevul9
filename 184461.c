static void freeGlobals(void)
{
	free(newPath);
	newPath = NULL;
	
	free(j9binBuffer);
	j9binBuffer = NULL;

	free(jrebinBuffer);
	jrebinBuffer = NULL;

	free(j9libBuffer);
	j9libBuffer = NULL;

	free(j9libvmBuffer);
	j9libvmBuffer = NULL;
	
	free(j9Buffer);
	j9Buffer = NULL;
}