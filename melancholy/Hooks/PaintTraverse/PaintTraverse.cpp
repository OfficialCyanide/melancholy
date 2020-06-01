#include "PaintTraverse.h"

using fn = void(__thiscall *)(void *, unsigned int, bool, bool);

void __fastcall PaintTraverse::Hook(void *panels, int edx, unsigned int vgui_panel, bool force_repaint, bool allow_force)
{
	if (gESP.NoScope)
	{
		const char *szHudScope = gInts.Panels->GetName(vgui_panel);

		if (szHudScope[0] == 'H' && szHudScope[3] == 'S' && szHudScope[8] == '\0')
			return;
	}

	VMTManager &hk = VMTManager::GetHook(panels);
	hk.GetMethod<fn>(Index)(panels, vgui_panel, force_repaint, allow_force);

	static unsigned int top_panel;

	if (top_panel == 0) {
		const char *name = gInts.Panels->GetName(vgui_panel);

		if (strstr(name, "FocusOverlayPanel"))
			top_panel = vgui_panel;
	}

	if (top_panel == vgui_panel)
	{
		static bool init_screen_size = false;

		if (!init_screen_size) {
			if (!gScreenSize.w || !gScreenSize.h)
				gScreenSize.Update();

			else init_screen_size = true;
		}

		if (!init_screen_size)
			return;

		CBaseEntity *local = gInts.EntityList->GetClientEntity(gInts.Engine->GetLocalPlayer());

		if (local && local->IsAlive())
		{
			local->ForceTauntCam(gESP.Thirdperson);
		}

		gESP.Run(local);

		if (!gInts.Engine->IsConnected() && !gInts.Engine->IsDrawingLoadingImage()) 
		{
			gInts.Panels->SetTopmostPopup(vgui_panel, true);

			static float x = -150.0f, y = 200.0f;

			x += (50.0f * gInts.Globals->interval_per_tick);
			y += ((sinf(gInts.Globals->curtime) * 15.0f) * gInts.Globals->interval_per_tick);

			static const char *s0 = "melancholy";
			int w0 = 0, h0 = 0;
			gInts.Surface->GetTextSize(gESP.DrawMark.dwFont, Utils::ToWC(s0), w0, h0);

			static const char *s1 = "UC release";
			int w1 = 0, h1 = 0;
			gInts.Surface->GetTextSize(gESP.DrawMark.dwFont, Utils::ToWC(s1), w1, h1);

			if ((static_cast<int>(x) - w0) > gScreenSize.w) {
				x = -150.0f;
			}

			RGBA_t clr = Utils::Rainbow();

			gESP.DrawMark.String((static_cast<int>(x) - (w0 / 2.0f)), (static_cast<int>(y) - (h0 / 2.0f)), clr, s0);
			gESP.DrawMark.String((static_cast<int>(x) - (w1 / 2.0f)), (static_cast<int>(y) - (h1 / 2.0f) + 16.0f), clr, s1);
		}
	}
}