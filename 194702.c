static void textview_write_body(TextView *textview, MimeInfo *mimeinfo)
{
	FILE *tmpfp;
	gchar buf[BUFFSIZE];
	CodeConverter *conv;
	const gchar *charset;
#ifndef G_OS_WIN32
	const gchar *p, *cmd;
#endif
	GSList *cur;
	gboolean continue_write = TRUE;
	size_t wrote = 0, i = 0;

	if (textview->messageview->forced_charset)
		charset = textview->messageview->forced_charset;
	else {
		/* use supersets transparently when possible */
		charset = procmime_mimeinfo_get_parameter(mimeinfo, "charset");
		if (charset && !strcasecmp(charset, CS_ISO_8859_1))
			charset = CS_WINDOWS_1252;
		else if (charset && !strcasecmp(charset, CS_X_GBK))
			charset = CS_GB18030;
		else if (charset && !strcasecmp(charset, CS_GBK))
			charset = CS_GB18030;
		else if (charset && !strcasecmp(charset, CS_GB2312))
			charset = CS_GB18030;
	}

	textview_set_font(textview, charset);

	conv = conv_code_converter_new(charset);

	procmime_force_encoding(textview->messageview->forced_encoding);
	
	textview->is_in_signature = FALSE;
	textview->is_diff = FALSE;
	textview->is_attachment = FALSE;;
	textview->is_in_git_patch = FALSE;

	procmime_decode_content(mimeinfo);

	account_sigsep_matchlist_create();

	if (!g_ascii_strcasecmp(mimeinfo->subtype, "html") &&
	    prefs_common.render_html) {
		gchar *filename;
		
		filename = procmime_get_tmp_file_name(mimeinfo);
		if (procmime_get_part(filename, mimeinfo) == 0) {
			tmpfp = claws_fopen(filename, "rb");
			if (tmpfp) {
				textview_show_html(textview, tmpfp, conv);
				claws_fclose(tmpfp);
			}
			claws_unlink(filename);
		}
		g_free(filename);
	} else if (!g_ascii_strcasecmp(mimeinfo->subtype, "enriched")) {
		gchar *filename;
		
		filename = procmime_get_tmp_file_name(mimeinfo);
		if (procmime_get_part(filename, mimeinfo) == 0) {
			tmpfp = claws_fopen(filename, "rb");
			if (tmpfp) {
				textview_show_ertf(textview, tmpfp, conv);
				claws_fclose(tmpfp);
			}
			claws_unlink(filename);
		}
		g_free(filename);
#ifndef G_OS_WIN32
	} else if ( g_ascii_strcasecmp(mimeinfo->subtype, "plain") &&
		   (cmd = prefs_common.mime_textviewer) && *cmd &&
		   (p = strchr(cmd, '%')) && *(p + 1) == 's') {
		int pid, pfd[2];
		const gchar *fname;

		fname  = procmime_get_tmp_file_name(mimeinfo);
		if (procmime_get_part(fname, mimeinfo)) goto textview_default;

		g_snprintf(buf, sizeof(buf), cmd, fname);
		debug_print("Viewing text content of type: %s (length: %d) "
			"using %s\n", mimeinfo->subtype, mimeinfo->length, buf);

		if (pipe(pfd) < 0) {
			g_snprintf(buf, sizeof(buf),
				"pipe failed for textview\n\n%s\n", g_strerror(errno));
			textview_write_line(textview, buf, conv, TRUE);
			goto textview_default;
		}
		pid = fork();
		if (pid < 0) {
			g_snprintf(buf, sizeof(buf),
				"fork failed for textview\n\n%s\n", g_strerror(errno));
			textview_write_line(textview, buf, conv, TRUE);
			close(pfd[0]);
			close(pfd[1]);
			goto textview_default;
		}
		if (pid == 0) { /* child */
			int rc;
			gchar **argv;
			argv = strsplit_with_quote(buf, " ", 0);
			close(1);
			close(pfd[0]);
			rc = dup(pfd[1]);
			rc = execvp(argv[0], argv);
			perror("execvp");
			close(pfd[1]);
			g_print(_("The command to view attachment "
			        "as text failed:\n"
			        "    %s\n"
			        "Exit code %d\n"), buf, rc);
			exit(255);
		}
		close(pfd[1]);
		tmpfp = claws_fdopen(pfd[0], "rb");
		while (claws_fgets(buf, sizeof(buf), tmpfp)) {
			textview_write_line(textview, buf, conv, TRUE);
			
			if (textview->stop_loading) {
				claws_fclose(tmpfp);
				waitpid(pid, pfd, 0);
				g_unlink(fname);
				account_sigsep_matchlist_delete();
				return;
			}
		}

		claws_fclose(tmpfp);
		waitpid(pid, pfd, 0);
		g_unlink(fname);
#endif
	} else {
#ifndef G_OS_WIN32
textview_default:
#endif
		if (!g_ascii_strcasecmp(mimeinfo->subtype, "x-patch")
				|| !g_ascii_strcasecmp(mimeinfo->subtype, "x-diff"))
			textview->is_diff = TRUE;

		/* Displayed part is an attachment, but not an attached
		 * e-mail. Set a flag, so that elsewhere in the code we
		 * know not to try making collapsible quotes in it. */
		if (mimeinfo->disposition == DISPOSITIONTYPE_ATTACHMENT &&
				mimeinfo->type != MIMETYPE_MESSAGE)
			textview->is_attachment = TRUE;

		if (mimeinfo->content == MIMECONTENT_MEM)
			tmpfp = str_open_as_stream(mimeinfo->data.mem);
		else
			tmpfp = claws_fopen(mimeinfo->data.filename, "rb");
		if (!tmpfp) {
			FILE_OP_ERROR(mimeinfo->data.filename, "claws_fopen");
			account_sigsep_matchlist_delete();
			return;
		}
		if (fseek(tmpfp, mimeinfo->offset, SEEK_SET) < 0) {
			FILE_OP_ERROR(mimeinfo->data.filename, "fseek");
			claws_fclose(tmpfp);
			account_sigsep_matchlist_delete();
			return;
		}
		debug_print("Viewing text content of type: %s (length: %d)\n", mimeinfo->subtype, mimeinfo->length);
		while (((i = ftell(tmpfp)) < mimeinfo->offset + mimeinfo->length) &&
		       (claws_fgets(buf, sizeof(buf), tmpfp) != NULL)
		       && continue_write) {
			textview_write_line(textview, buf, conv, TRUE);
			if (textview->stop_loading) {
				claws_fclose(tmpfp);
				account_sigsep_matchlist_delete();
				return;
			}
			wrote += ftell(tmpfp)-i;
			if (mimeinfo->length > 1024*1024 
			&&  wrote > 1024*1024
			&& !textview->messageview->show_full_text) {
				continue_write = FALSE;
			}
		}
		claws_fclose(tmpfp);
	}

	account_sigsep_matchlist_delete();

	conv_code_converter_destroy(conv);
	procmime_force_encoding(0);

	textview->uri_list = g_slist_reverse(textview->uri_list);
	for (cur = textview->uri_list; cur; cur = cur->next) {
		ClickableText *uri = (ClickableText *)cur->data;
		if (!uri->is_quote)
			continue;
		if (!prefs_common.hide_quotes ||
		    uri->quote_level+1 < prefs_common.hide_quotes) {
			textview_toggle_quote(textview, cur, uri, TRUE);
			if (textview->stop_loading) {
				return;
			}
		}
	}
	
	if (continue_write == FALSE) {
		messageview_show_partial_display(
			textview->messageview, 
			textview->messageview->msginfo,
			mimeinfo->length);
	}
	GTK_EVENTS_FLUSH();
}