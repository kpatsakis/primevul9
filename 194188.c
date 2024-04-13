static rsRetVal MsgSerialize(msg_t *pThis, strm_t *pStrm)
{
	uchar *psz;
	int len;
	DEFiRet;

	assert(pThis != NULL);
	assert(pStrm != NULL);

	/* then serialize elements */
	CHKiRet(obj.BeginSerialize(pStrm, (obj_t*) pThis));
	objSerializeSCALAR(pStrm, iProtocolVersion, SHORT);
	objSerializeSCALAR(pStrm, iSeverity, SHORT);
	objSerializeSCALAR(pStrm, iFacility, SHORT);
	objSerializeSCALAR(pStrm, msgFlags, INT);
	objSerializeSCALAR(pStrm, ttGenTime, INT);
	objSerializeSCALAR(pStrm, tRcvdAt, SYSLOGTIME);
	objSerializeSCALAR(pStrm, tTIMESTAMP, SYSLOGTIME);
	/* enable this, if someone actually uses UxTradMsg, delete after some  time has
	 * passed and nobody complained -- rgerhards, 2009-06-16
	objSerializeSCALAR(pStrm, offsAfterPRI, SHORT);
	*/

	CHKiRet(obj.SerializeProp(pStrm, UCHAR_CONSTANT("pszTAG"), PROPTYPE_PSZ, (void*)
		((pThis->iLenTAG < CONF_TAG_BUFSIZE) ? pThis->TAG.szBuf : pThis->TAG.pszTAG)));

	objSerializePTR(pStrm, pszRawMsg, PSZ);
	objSerializePTR(pStrm, pszHOSTNAME, PSZ);
	getInputName(pThis, &psz, &len);
	CHKiRet(obj.SerializeProp(pStrm, UCHAR_CONSTANT("pszInputName"), PROPTYPE_PSZ, (void*) psz));
	psz = getRcvFrom(pThis); 
	CHKiRet(obj.SerializeProp(pStrm, UCHAR_CONSTANT("pszRcvFrom"), PROPTYPE_PSZ, (void*) psz));
	psz = getRcvFromIP(pThis); 
	CHKiRet(obj.SerializeProp(pStrm, UCHAR_CONSTANT("pszRcvFromIP"), PROPTYPE_PSZ, (void*) psz));

	objSerializePTR(pStrm, pCSStrucData, CSTR);
	objSerializePTR(pStrm, pCSAPPNAME, CSTR);
	objSerializePTR(pStrm, pCSPROCID, CSTR);
	objSerializePTR(pStrm, pCSMSGID, CSTR);

	/* offset must be serialized after pszRawMsg, because we need that to obtain the correct
	 * MSG size.
	 */
	objSerializeSCALAR(pStrm, offMSG, SHORT);

	CHKiRet(obj.EndSerialize(pStrm));

finalize_it:
	RETiRet;
}