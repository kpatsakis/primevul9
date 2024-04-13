static void construct_reply_common(struct smb_request *req, const char *inbuf,
				   char *outbuf)
{
	srv_set_message(outbuf,0,0,false);
	
	SCVAL(outbuf, smb_com, req->cmd);
	SIVAL(outbuf,smb_rcls,0);
	SCVAL(outbuf,smb_flg, FLAG_REPLY | (CVAL(inbuf,smb_flg) & FLAG_CASELESS_PATHNAMES)); 
	SSVAL(outbuf,smb_flg2,
		(SVAL(inbuf,smb_flg2) & FLAGS2_UNICODE_STRINGS) |
		common_flags2);
	memset(outbuf+smb_pidhigh,'\0',(smb_tid-smb_pidhigh));

	SSVAL(outbuf,smb_tid,SVAL(inbuf,smb_tid));
	SSVAL(outbuf,smb_pid,SVAL(inbuf,smb_pid));
	SSVAL(outbuf,smb_uid,SVAL(inbuf,smb_uid));
	SSVAL(outbuf,smb_mid,SVAL(inbuf,smb_mid));
}