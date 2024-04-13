OFCondition DcmSCP::setConfig(const DcmSCPConfig& config)
{
  if (isConnected())
  {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_cfg = DcmSharedSCPConfig( config );
  return EC_Normal;
}