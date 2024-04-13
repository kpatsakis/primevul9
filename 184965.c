void CClient::AutoScreenshot_Start()
{
	if(g_Config.m_ClAutoScreenshot)
	{
		Graphics()->TakeScreenshot("auto/autoscreen");
		m_AutoScreenshotRecycle = true;
	}
}