static inline bool hidpp_report_is_connect_event(struct hidpp_report *report)
{
	return (report->report_id == REPORT_ID_HIDPP_SHORT) &&
		(report->rap.sub_id == 0x41);
}