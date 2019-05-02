#include "D3DMenu.h"

bool bShowWindow = true;
bool bCheckBox1 = true;
bool bCheckBox2 = false;

void DrawD3DMenuMain()
{
	DrawD3DMenu(10, 20, 250, 600);
	KeyBoard();
}

void DrawD3DMenu(float x, float y, float width, float height)
{
	ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowBgAlpha(0.7f);

	if (bShowWindow)
	{
		ImGui::Begin(u8"by£∫Zllf   ≤‚ ‘", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Text(u8"fps : %d", (int)ImGui::GetIO().Framerate);
		ImGui::Checkbox(u8"≤‚ ‘ 1", &bCheckBox1);
		ImGui::SameLine();
		ImGui::Checkbox(u8"≤‚ ‘ 2", &bCheckBox2);
		ImGui::End();
	}
}

void KeyBoard()
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		bShowWindow = !bShowWindow;
}