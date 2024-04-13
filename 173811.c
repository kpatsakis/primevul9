static inline bool hidpp_match_answer(struct hidpp_report *question,
		struct hidpp_report *answer)
{
	return (answer->fap.feature_index == question->fap.feature_index) &&
	   (answer->fap.funcindex_clientid == question->fap.funcindex_clientid);
}