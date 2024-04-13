static int round_event_name_len(struct fsnotify_event *fsn_event)
{
	struct inotify_event_info *event;

	event = INOTIFY_E(fsn_event);
	if (!event->name_len)
		return 0;
	return roundup(event->name_len + 1, sizeof(struct inotify_event));
}