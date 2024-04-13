	MODCONSTRUCTOR(CBounceDCCMod) {
		AddHelpCommand();
		AddCommand("ListDCCs", static_cast<CModCommand::ModCmdFunc>(&CBounceDCCMod::ListDCCsCommand),
			"", "List all active DCCs");
		AddCommand("UseClientIP", static_cast<CModCommand::ModCmdFunc>(&CBounceDCCMod::UseClientIPCommand),
			"<true|false>");
	}