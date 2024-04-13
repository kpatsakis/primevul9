uchar*  rsCStrGetSzStr(cstr_t *pThis)
{
	size_t i;

	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);

	if(pThis->pBuf != NULL)
		if(pThis->pszBuf == NULL) {
			/* we do not yet have a usable sz version - so create it... */
			if((pThis->pszBuf = malloc((pThis->iStrLen + 1) * sizeof(uchar))) == NULL) {
				/* TODO: think about what to do - so far, I have no bright
				 *       idea... rgerhards 2005-09-07
				 */
			}
			else { /* we can create the sz String */
				/* now copy it while doing a sanity check. The string might contain a
				 * \0 byte. There is no way how a sz string can handle this. For
				 * the time being, we simply replace it with space - something that
				 * could definitely be improved (TODO).
				 * 2005-09-15 rgerhards
				 */
				for(i = 0 ; i < pThis->iStrLen ; ++i) {
					if(pThis->pBuf[i] == '\0')
						pThis->pszBuf[i] = ' ';
					else
						pThis->pszBuf[i] = pThis->pBuf[i];
				}
				/* write terminator... */
				pThis->pszBuf[i] = '\0';
			}
		}

	return(pThis->pszBuf);
}