	virtual EModRet OnUserCTCP(CString& sTarget, CString& sMessage) {
		if (sMessage.Equals("DCC ", false, 4)) {
			CString sType = sMessage.Token(1);
			CString sFile = sMessage.Token(2);
			unsigned long uLongIP = sMessage.Token(3).ToULong();
			unsigned short uPort = sMessage.Token(4).ToUShort();
			unsigned long uFileSize = sMessage.Token(5).ToULong();
			CString sIP = GetLocalDCCIP();

			if (!UseClientIP()) {
				uLongIP = CUtils::GetLongIP(m_pClient->GetRemoteIP());
			}

			if (sType.Equals("CHAT")) {
				unsigned short uBNCPort = CDCCBounce::DCCRequest(sTarget, uLongIP, uPort, "", true, this, "");
				if (uBNCPort) {
					PutIRC("PRIVMSG " + sTarget + " :\001DCC CHAT chat " + CString(CUtils::GetLongIP(sIP)) + " " + CString(uBNCPort) + "\001");
				}
			} else if (sType.Equals("SEND")) {
				// DCC SEND readme.txt 403120438 5550 1104
				unsigned short uBNCPort = CDCCBounce::DCCRequest(sTarget, uLongIP, uPort, sFile, false, this, "");
				if (uBNCPort) {
					PutIRC("PRIVMSG " + sTarget + " :\001DCC SEND " + sFile + " " + CString(CUtils::GetLongIP(sIP)) + " " + CString(uBNCPort) + " " + CString(uFileSize) + "\001");
				}
			} else if (sType.Equals("RESUME")) {
				// PRIVMSG user :DCC RESUME "znc.o" 58810 151552
				unsigned short uResumePort = sMessage.Token(3).ToUShort();

				set<CSocket*>::const_iterator it;
				for (it = BeginSockets(); it != EndSockets(); ++it) {
					CDCCBounce* pSock = (CDCCBounce*) *it;

					if (pSock->GetLocalPort() == uResumePort) {
						PutIRC("PRIVMSG " + sTarget + " :\001DCC " + sType + " " + sFile + " " + CString(pSock->GetUserPort()) + " " + sMessage.Token(4) + "\001");
					}
				}
			} else if (sType.Equals("ACCEPT")) {
				// Need to lookup the connection by port, filter the port, and forward to the user

				set<CSocket*>::const_iterator it;
				for (it = BeginSockets(); it != EndSockets(); ++it) {
					CDCCBounce* pSock = (CDCCBounce*) *it;
					if (pSock->GetUserPort() == sMessage.Token(3).ToUShort()) {
						PutIRC("PRIVMSG " + sTarget + " :\001DCC " + sType + " " + sFile + " " + CString(pSock->GetLocalPort()) + " " + sMessage.Token(4) + "\001");
					}
				}
			}

			return HALTCORE;
		}

		return CONTINUE;
	}