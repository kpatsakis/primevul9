static unsigned int get_protocol_family(unsigned int protocol)
{
  unsigned int family;

  switch(protocol) {
  case CURLPROTO_HTTP:
  case CURLPROTO_HTTPS:
    family = CURLPROTO_HTTP;
    break;

  case CURLPROTO_FTP:
  case CURLPROTO_FTPS:
    family = CURLPROTO_FTP;
    break;

  case CURLPROTO_SCP:
    family = CURLPROTO_SCP;
    break;

  case CURLPROTO_SFTP:
    family = CURLPROTO_SFTP;
    break;

  case CURLPROTO_TELNET:
    family = CURLPROTO_TELNET;
    break;

  case CURLPROTO_LDAP:
  case CURLPROTO_LDAPS:
    family = CURLPROTO_LDAP;
    break;

  case CURLPROTO_DICT:
    family = CURLPROTO_DICT;
    break;

  case CURLPROTO_FILE:
    family = CURLPROTO_FILE;
    break;

  case CURLPROTO_TFTP:
    family = CURLPROTO_TFTP;
    break;

  case CURLPROTO_IMAP:
  case CURLPROTO_IMAPS:
    family = CURLPROTO_IMAP;
    break;

  case CURLPROTO_POP3:
  case CURLPROTO_POP3S:
    family = CURLPROTO_POP3;
    break;

  case CURLPROTO_SMTP:
  case CURLPROTO_SMTPS:
      family = CURLPROTO_SMTP;
      break;

  case CURLPROTO_RTSP:
    family = CURLPROTO_RTSP;
    break;

  case CURLPROTO_RTMP:
  case CURLPROTO_RTMPS:
    family = CURLPROTO_RTMP;
    break;

  case CURLPROTO_RTMPT:
  case CURLPROTO_RTMPTS:
    family = CURLPROTO_RTMPT;
    break;

  case CURLPROTO_RTMPE:
    family = CURLPROTO_RTMPE;
    break;

  case CURLPROTO_RTMPTE:
    family = CURLPROTO_RTMPTE;
    break;

  case CURLPROTO_GOPHER:
    family = CURLPROTO_GOPHER;
    break;

  case CURLPROTO_SMB:
  case CURLPROTO_SMBS:
    family = CURLPROTO_SMB;
    break;

  default:
      family = 0;
      break;
  }

  return family;
}