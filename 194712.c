static void recursive_add_parts(TextView *textview, GNode *node)
{
        GNode * iter;
	MimeInfo *mimeinfo;
        START_TIMING("");

        mimeinfo = (MimeInfo *) node->data;
        
        textview_add_part(textview, mimeinfo);
#ifdef GENERIC_UMPC
	textview_set_position(textview, 0);
#endif        
        if ((mimeinfo->type != MIMETYPE_MULTIPART) &&
            (mimeinfo->type != MIMETYPE_MESSAGE)) {
	    	END_TIMING();
                return;
        }
        if (g_ascii_strcasecmp(mimeinfo->subtype, "alternative") == 0) {
                GNode * preferred_body;
                int preferred_score;

                /*
                  text/plain : score 3
                  text/ *    : score 2
                  other      : score 1
                */
                preferred_body = NULL;
                preferred_score = 0;

                for (iter = g_node_first_child(node) ; iter != NULL ;
                     iter = g_node_next_sibling(iter)) {
                        int score;
                        MimeInfo * submime;

                        score = 1;
                        submime = (MimeInfo *) iter->data;
                        if (submime->type == MIMETYPE_TEXT)
                                score = 2;
 
                        if (submime->subtype != NULL) {
                                if (g_ascii_strcasecmp(submime->subtype, "plain") == 0)
                                        score = 3;
                        }

                        if (score > preferred_score) {
                                preferred_score = score;
                                preferred_body = iter;
                        }
                }

                if (preferred_body != NULL) {
                        recursive_add_parts(textview, preferred_body);
                }
        }
        else {
                for (iter = g_node_first_child(node) ; iter != NULL ;
                     iter = g_node_next_sibling(iter)) {
                        recursive_add_parts(textview, iter);
                }
        }
	END_TIMING();
}