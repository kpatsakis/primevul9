void check_reload(time_t t)
{
	time_t printcap_cache_time = (time_t)lp_printcap_cache_time();

	if(last_smb_conf_reload_time == 0) {
		last_smb_conf_reload_time = t;
		/* Our printing subsystem might not be ready at smbd start up.
		   Then no printer is available till the first printers check
		   is performed.  A lower initial interval circumvents this. */
		if ( printcap_cache_time > 60 )
			last_printer_reload_time = t - printcap_cache_time + 60;
		else
			last_printer_reload_time = t;
	}

	if (mypid != getpid()) { /* First time or fork happened meanwhile */
		/* randomize over 60 second the printcap reload to avoid all
		 * process hitting cupsd at the same time */
		int time_range = 60;

		last_printer_reload_time += random() % time_range;
		mypid = getpid();
	}

	if (t >= last_smb_conf_reload_time+SMBD_RELOAD_CHECK) {
		reload_services(True);
		last_smb_conf_reload_time = t;
	}

	/* 'printcap cache time = 0' disable the feature */
	
	if ( printcap_cache_time != 0 )
	{ 
		/* see if it's time to reload or if the clock has been set back */
		
		if ( (t >= last_printer_reload_time+printcap_cache_time) 
			|| (t-last_printer_reload_time  < 0) ) 
		{
			DEBUG( 3,( "Printcap cache time expired.\n"));
			reload_printers();
			last_printer_reload_time = t;
		}
	}
}