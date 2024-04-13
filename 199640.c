void CDCCBounce::PutServ(const CString& sLine) {
	DEBUG(GetSockName() << " -> [" << sLine << "]");
	Write(sLine + "\r\n");
}