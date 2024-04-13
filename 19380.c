bool PlayerGeneric::grabChannelInfo(mp_sint32 chn, TPlayerChannelInfo& channelInfo) const
{
	if (player)
		return player->grabChannelInfo(chn, channelInfo);
		
	return false;
}