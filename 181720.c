static char* graph_cmd(RCore *core, char *r2_cmd, const char *save_path) {
	const char *dot = "dot";
	char *cmd = NULL;
	const char *ext = r_config_get (core->config, "graph.gv.format");
	char *dotPath = r_file_path (dot);
	if (!strcmp (dotPath, dot)) {
		free (dotPath);
		dot = "xdot";
		dotPath = r_file_path (dot);
		if (!strcmp (dotPath, dot)) {
			free (dotPath);
			return r_str_new ("agf");
		}
	}
	if (save_path && *save_path) {
		cmd = r_str_newf ("%s > a.dot;!%s -T%s -o%s a.dot;",
			r2_cmd, dot, ext, save_path);
	} else {
		char *viewer = getViewerPath();
		if (viewer) {
			cmd = r_str_newf ("%s > a.dot;!%s -T%s -oa.%s a.dot;!%s a.%s",
				r2_cmd, dot, ext, ext, viewer, ext);
			free (viewer);
		} else {
			eprintf ("Cannot find a valid picture viewer\n");
		}
	}
	free (dotPath);
	return cmd;
}