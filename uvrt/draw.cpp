#include"auto.h"

void ����ֱ��(ImVec2 pos, ImVec2 pos2, ImColor color, float size) {

	ImGui::GetForegroundDrawList()->AddLine(pos,pos2,color,size);

}
