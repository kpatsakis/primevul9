QString loadErrorHandlingToStr(LoadPage::LoadErrorHandling leh) {
	switch (leh) {
	case LoadPage::abort: return "abort";
	case LoadPage::skip: return "skip";
	case LoadPage::ignore: return "ignore";
	}
	throw std::logic_error("Internal error in loadErrorHandlingToStr");
}