	void UseClientIPCommand(const CString& sLine) {
		CString sValue = sLine.Token(1, true);

		if (!sValue.empty()) {
			SetNV("UseClientIP", sValue);
		}

		PutModule("UseClientIP: " + CString(GetNV("UseClientIP").ToBool()));
	}