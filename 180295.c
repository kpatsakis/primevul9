_zip_dirent_torrent_normalize(struct zip_dirent *de)
{
    static struct tm torrenttime;
    static time_t last_mod = 0;

    if (last_mod == 0) {
#ifdef HAVE_STRUCT_TM_TM_ZONE
	time_t now;
	struct tm *l;
#endif

	torrenttime.tm_sec = 0;
	torrenttime.tm_min = 32;
	torrenttime.tm_hour = 23;
	torrenttime.tm_mday = 24;
	torrenttime.tm_mon = 11;
	torrenttime.tm_year = 96;
	torrenttime.tm_wday = 0;
	torrenttime.tm_yday = 0;
	torrenttime.tm_isdst = 0;

#ifdef HAVE_STRUCT_TM_TM_ZONE
	time(&now);
	l = localtime(&now);
	torrenttime.tm_gmtoff = l->tm_gmtoff;
	torrenttime.tm_zone = l->tm_zone;
#endif

	last_mod = mktime(&torrenttime);
    }
    
    de->version_madeby = 0;
    de->version_needed = 20; /* 2.0 */
    de->bitflags = 2; /* maximum compression */
    de->comp_method = ZIP_CM_DEFLATE;
    de->last_mod = last_mod;

    de->disk_number = 0;
    de->int_attrib = 0;
    de->ext_attrib = 0;
    de->offset = 0;

    free(de->extrafield);
    de->extrafield = NULL;
    de->extrafield_len = 0;
    free(de->comment);
    de->comment = NULL;
    de->comment_len = 0;
}