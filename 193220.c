static const char *applicationType(Uint32 i)
{
  if (i==DVPSIPCMessage::clientOther) return "unspecified";
  if (i==DVPSIPCMessage::clientStoreSCP) return "Receiver (Store SCP)";
  if (i==DVPSIPCMessage::clientStoreSCU) return "Sender (Store SCU)";
  if (i==DVPSIPCMessage::clientPrintSCP) return "Printer (Print SCP)";
  if (i==DVPSIPCMessage::clientPrintSCU) return "Print Client (Print SCU)";
  if (i==DVPSIPCMessage::clientQRSCP) return "Database (Query/Retrieve SCP)";
  return "unknown application type";
}