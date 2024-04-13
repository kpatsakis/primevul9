int CClient::LoadData()
{
	m_DebugFont = Graphics()->LoadTexture("debug_font.png", IStorage::TYPE_ALL, CImageInfo::FORMAT_AUTO, IGraphics::TEXLOAD_NORESAMPLE);
	return 1;
}