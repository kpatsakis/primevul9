void CClient::DebugRender()
{
	static NETSTATS Prev, Current;
	static int64 LastSnap = 0;
	static float FrameTimeAvg = 0;
	int64 Now = time_get();
	char aBuffer[512];

	if(!g_Config.m_Debug)
		return;

	//m_pGraphics->BlendNormal();
	Graphics()->TextureSet(m_DebugFont);
	Graphics()->MapScreen(0,0,Graphics()->ScreenWidth(),Graphics()->ScreenHeight());
	Graphics()->QuadsBegin();

	if(time_get()-LastSnap > time_freq())
	{
		LastSnap = time_get();
		Prev = Current;
		net_stats(&Current);
	}

	/*
		eth = 14
		ip = 20
		udp = 8
		total = 42
	*/
	FrameTimeAvg = FrameTimeAvg*0.9f + m_RenderFrameTime*0.1f;
	str_format(aBuffer, sizeof(aBuffer), "ticks: %8d %8d mem %dk %d gfxmem: %dk fps: %3d",
		m_CurGameTick, m_PredTick,
		mem_stats()->allocated/1024,
		mem_stats()->total_allocations,
		Graphics()->MemoryUsage()/1024,
		(int)(1.0f/FrameTimeAvg + 0.5f));
	Graphics()->QuadsText(2, 2, 16, aBuffer);


	{
		int SendPackets = (Current.sent_packets-Prev.sent_packets);
		int SendBytes = (Current.sent_bytes-Prev.sent_bytes);
		int SendTotal = SendBytes + SendPackets*42;
		int RecvPackets = (Current.recv_packets-Prev.recv_packets);
		int RecvBytes = (Current.recv_bytes-Prev.recv_bytes);
		int RecvTotal = RecvBytes + RecvPackets*42;

		if(!SendPackets) SendPackets++;
		if(!RecvPackets) RecvPackets++;
		str_format(aBuffer, sizeof(aBuffer), "send: %3d %5d+%4d=%5d (%3d kbps) avg: %5d\nrecv: %3d %5d+%4d=%5d (%3d kbps) avg: %5d",
			SendPackets, SendBytes, SendPackets*42, SendTotal, (SendTotal*8)/1024, SendBytes/SendPackets,
			RecvPackets, RecvBytes, RecvPackets*42, RecvTotal, (RecvTotal*8)/1024, RecvBytes/RecvPackets);
		Graphics()->QuadsText(2, 14, 16, aBuffer);
	}

	// render rates
	{
		int y = 0;
		int i;
		for(i = 0; i < 256; i++)
		{
			if(m_SnapshotDelta.GetDataRate(i))
			{
				str_format(aBuffer, sizeof(aBuffer), "%4d %20s: %8d %8d %8d", i, GameClient()->GetItemName(i), m_SnapshotDelta.GetDataRate(i)/8, m_SnapshotDelta.GetDataUpdates(i),
					(m_SnapshotDelta.GetDataRate(i)/m_SnapshotDelta.GetDataUpdates(i))/8);
				Graphics()->QuadsText(2, 100+y*12, 16, aBuffer);
				y++;
			}
		}
	}

	str_format(aBuffer, sizeof(aBuffer), "pred: %d ms",
		(int)((m_PredictedTime.Get(Now)-m_GameTime.Get(Now))*1000/(float)time_freq()));
	Graphics()->QuadsText(2, 70, 16, aBuffer);
	Graphics()->QuadsEnd();

	// render graphs
	if(g_Config.m_DbgGraphs)
	{
		//Graphics()->MapScreen(0,0,400.0f,300.0f);
		float w = Graphics()->ScreenWidth()/4.0f;
		float h = Graphics()->ScreenHeight()/6.0f;
		float sp = Graphics()->ScreenWidth()/100.0f;
		float x = Graphics()->ScreenWidth()-w-sp;

		m_FpsGraph.ScaleMax();
		m_FpsGraph.ScaleMin();
		m_FpsGraph.Render(Graphics(), m_DebugFont, x, sp*5, w, h, "FPS");
		m_InputtimeMarginGraph.Render(Graphics(), m_DebugFont, x, sp*5+h+sp, w, h, "Prediction Margin");
		m_GametimeMarginGraph.Render(Graphics(), m_DebugFont, x, sp*5+h+sp+h+sp, w, h, "Gametime Margin");
	}
}