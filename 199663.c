unsigned short CDCCBounce::DCCRequest(const CString& sNick, unsigned long uLongIP, unsigned short uPort, const CString& sFileName, bool bIsChat, CBounceDCCMod* pMod, const CString& sRemoteIP) {
	CDCCBounce* pDCCBounce = new CDCCBounce(pMod, uLongIP, uPort, sFileName, sNick, sRemoteIP, bIsChat);
	unsigned short uListenPort = CZNC::Get().GetManager().ListenRand("DCC::" + CString((bIsChat) ? "Chat" : "Xfer") + "::Local::" + sNick,
			pMod->GetLocalDCCIP(), false, SOMAXCONN, pDCCBounce, 120);

	return uListenPort;
}