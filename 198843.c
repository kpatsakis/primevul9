void inotify_ignored_and_remove_idr(struct fsnotify_mark *fsn_mark,
				    struct fsnotify_group *group)
{
	struct inotify_inode_mark *i_mark;
	struct fsnotify_iter_info iter_info = { };

	fsnotify_iter_set_report_type_mark(&iter_info, FSNOTIFY_OBJ_TYPE_INODE,
					   fsn_mark);

	/* Queue ignore event for the watch */
	inotify_handle_event(group, NULL, FS_IN_IGNORED, NULL,
			     FSNOTIFY_EVENT_NONE, NULL, 0, &iter_info);

	i_mark = container_of(fsn_mark, struct inotify_inode_mark, fsn_mark);
	/* remove this mark from the idr */
	inotify_remove_from_idr(group, i_mark);

	dec_inotify_watches(group->inotify_data.ucounts);
}