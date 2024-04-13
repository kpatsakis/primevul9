respip_views_apply_cfg(struct views* vs, struct config_file* cfg,
	int* have_view_respip_cfg)
{
	struct config_view* cv;
	struct view* v;
	int ret;

	for(cv = cfg->views; cv; cv = cv->next) {

		/** if no respip config for this view then there's
		  * nothing to do; note that even though respip data must go
		  * with respip action, we're checking for both here because
		  * we want to catch the case where the respip action is missing
		  * while the data is present */
		if(!cv->respip_actions && !cv->respip_data)
			continue;

		if(!(v = views_find_view(vs, cv->name, 1))) {
			log_err("view '%s' unexpectedly missing", cv->name);
			return 0;
		}
		if(!v->respip_set) {
			v->respip_set = respip_set_create();
			if(!v->respip_set) {
				log_err("out of memory");
				lock_rw_unlock(&v->lock);
				return 0;
			}
		}
		ret = respip_set_apply_cfg(v->respip_set, NULL, 0, NULL,
			cv->respip_actions, cv->respip_data);
		lock_rw_unlock(&v->lock);
		if(!ret) {
			log_err("Error while applying respip configuration "
				"for view '%s'", cv->name);
			return 0;
		}
		*have_view_respip_cfg = (*have_view_respip_cfg ||
			v->respip_set->ip_tree.count);
		cv->respip_actions = NULL;
		cv->respip_data = NULL;
	}
	return 1;
}