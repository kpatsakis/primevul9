void CClient::Render()
{
	if(g_Config.m_GfxClear)
		Graphics()->Clear(1,1,0);

	GameClient()->OnRender();
	DebugRender();
}