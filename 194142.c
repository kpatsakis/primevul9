uchar *propIDToName(propid_t propID)
{
	switch(propID) {
		case PROP_MSG:
			return UCHAR_CONSTANT("msg");
		case PROP_TIMESTAMP:
			return UCHAR_CONSTANT("timestamp");
		case PROP_HOSTNAME:
			return UCHAR_CONSTANT("hostname");
		case PROP_SYSLOGTAG:
			return UCHAR_CONSTANT("syslogtag");
		case PROP_RAWMSG:
			return UCHAR_CONSTANT("rawmsg");
		/* enable this, if someone actually uses UxTradMsg, delete after some  time has
		 * passed and nobody complained -- rgerhards, 2009-06-16
		case PROP_UXTRADMSG:
			pRes = getUxTradMsg(pMsg);
			break;
		*/
		case PROP_INPUTNAME:
			return UCHAR_CONSTANT("inputname");
		case PROP_FROMHOST:
			return UCHAR_CONSTANT("fromhost");
		case PROP_FROMHOST_IP:
			return UCHAR_CONSTANT("fromhost-ip");
		case PROP_PRI:
			return UCHAR_CONSTANT("pri");
		case PROP_PRI_TEXT:
			return UCHAR_CONSTANT("pri-text");
		case PROP_IUT:
			return UCHAR_CONSTANT("iut");
		case PROP_SYSLOGFACILITY:
			return UCHAR_CONSTANT("syslogfacility");
		case PROP_SYSLOGFACILITY_TEXT:
			return UCHAR_CONSTANT("syslogfacility-text");
		case PROP_SYSLOGSEVERITY:
			return UCHAR_CONSTANT("syslogseverity");
		case PROP_SYSLOGSEVERITY_TEXT:
			return UCHAR_CONSTANT("syslogseverity-text");
		case PROP_TIMEGENERATED:
			return UCHAR_CONSTANT("timegenerated");
		case PROP_PROGRAMNAME:
			return UCHAR_CONSTANT("programname");
		case PROP_PROTOCOL_VERSION:
			return UCHAR_CONSTANT("protocol-version");
		case PROP_STRUCTURED_DATA:
			return UCHAR_CONSTANT("structured-data");
		case PROP_APP_NAME:
			return UCHAR_CONSTANT("app-name");
		case PROP_PROCID:
			return UCHAR_CONSTANT("procid");
		case PROP_MSGID:
			return UCHAR_CONSTANT("msgid");
		case PROP_SYS_NOW:
			return UCHAR_CONSTANT("$NOW");
		case PROP_SYS_YEAR:
			return UCHAR_CONSTANT("$YEAR");
		case PROP_SYS_MONTH:
			return UCHAR_CONSTANT("$MONTH");
		case PROP_SYS_DAY:
			return UCHAR_CONSTANT("$DAY");
		case PROP_SYS_HOUR:
			return UCHAR_CONSTANT("$HOUR");
		case PROP_SYS_HHOUR:
			return UCHAR_CONSTANT("$HHOUR");
		case PROP_SYS_QHOUR:
			return UCHAR_CONSTANT("$QHOUR");
		case PROP_SYS_MINUTE:
			return UCHAR_CONSTANT("$MINUTE");
		case PROP_SYS_MYHOSTNAME:
			return UCHAR_CONSTANT("$MYHOSTNAME");
		default:
			return UCHAR_CONSTANT("*invalid property id*");
	}
}