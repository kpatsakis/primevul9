static const char *statusString(Uint32 i)
{
  if (i==DVPSIPCMessage::statusOK) return "OK";
  if (i==DVPSIPCMessage::statusWarning) return "Warning";
  if (i==DVPSIPCMessage::statusError) return "Error";
  return "unknown status code";
}