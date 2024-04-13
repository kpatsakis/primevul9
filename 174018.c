GenFingerprintStr(char *pFingerprint, int sizeFingerprint, char *fpBuf)
{
	int iSrc, iDst;

	fpBuf[0] = 'S', fpBuf[1] = 'H', fpBuf[2] = 'A'; fpBuf[3] = '1';
	// TODO: length check fo fpBuf (but far from being urgent...)
	for(iSrc = 0, iDst = 4 ; iSrc < sizeFingerprint ; ++iSrc, iDst += 3) {
		sprintf(fpBuf+iDst, ":%2.2X", (unsigned char) pFingerprint[iSrc]);
	}
}