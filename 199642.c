void CDCCBounce::ReadLine(const CString& sData) {
	CString sLine = sData.TrimRight_n("\r\n");

	DEBUG(GetSockName() << " <- [" << sLine << "]");

	PutPeer(sLine);
}