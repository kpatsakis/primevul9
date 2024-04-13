
static void set_timezone_from_timelib_time(php_timezone_obj *tzobj, timelib_time *t)
{
       tzobj->initialized = 1;
       tzobj->type = t->zone_type;
       switch (t->zone_type) {
               case TIMELIB_ZONETYPE_ID:
                       tzobj->tzi.tz = t->tz_info;
                       break;
               case TIMELIB_ZONETYPE_OFFSET:
                       tzobj->tzi.utc_offset = t->z;
                       break;
               case TIMELIB_ZONETYPE_ABBR:
                       tzobj->tzi.z.utc_offset = t->z;
                       tzobj->tzi.z.dst = t->dst;
                       tzobj->tzi.z.abbr = strdup(t->tz_abbr);
                       break;
       }