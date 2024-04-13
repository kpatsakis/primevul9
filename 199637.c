	void ListDCCsCommand(const CString& sLine) {
		CTable Table;
		Table.AddColumn("Type");
		Table.AddColumn("State");
		Table.AddColumn("Speed");
		Table.AddColumn("Nick");
		Table.AddColumn("IP");
		Table.AddColumn("File");

		set<CSocket*>::const_iterator it;
		for (it = BeginSockets(); it != EndSockets(); ++it) {
			CDCCBounce* pSock = (CDCCBounce*) *it;
			CString sSockName = pSock->GetSockName();

			if (!(pSock->IsRemote())) {
				Table.AddRow();
				Table.SetCell("Nick", pSock->GetRemoteNick());
				Table.SetCell("IP", pSock->GetRemoteIP());

				if (pSock->IsChat()) {
					Table.SetCell("Type", "Chat");
				} else {
					Table.SetCell("Type", "Xfer");
					Table.SetCell("File", pSock->GetFileName());
				}

				CString sState = "Waiting";
				if ((pSock->IsConnected()) || (pSock->IsPeerConnected())) {
					sState = "Halfway";
					if ((pSock->IsPeerConnected()) && (pSock->IsPeerConnected())) {
						sState = "Connected";
					}
				}
				Table.SetCell("State", sState);
			}
		}

		if (PutModule(Table) == 0) {
			PutModule("You have no active DCCs.");
		}
	}