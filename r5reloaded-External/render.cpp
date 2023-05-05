#include "render.h"
#include <string>

extern Memory m;

// ESP Options
bool TeamESP = true;
bool DummyESP = false;
bool ESP_Box = true;
bool ESP_Line = false;
bool ESP_HealthBar = true;
bool ESP_Distance = false;
float ESP_MaxDist = 300;
int MAX_COUNT = 16000;
ImVec4 ESP_Color = { 1.f, 1.f, 1.f, 1.f };

uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent);
Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition);
bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to);

// Info
void render::m_Info()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)g.GameSize.right, (float)g.GameSize.bottom));
    ImGui::Begin("##Info", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImGui::Text("[ unknowncheats.me ] R5R-External");
    ImGui::Text("Overlay FPS : %.1f", ImGui::GetIO().Framerate);

    if (DummyESP)
        MAX_COUNT = 16000;
    else
        MAX_COUNT = 128;

    ImGui::End();
}

// メニュー
void render::m_Menu()
{
    ImGui::SetNextWindowSize(ImVec2(500.f, 500.f));
    ImGui::Begin("R5Reloaded ESP [ EXTERNAL ]", (bool*)NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

    ImGui::PushItemWidth(200.f);

    ImGui::NewLine();

    if (ImGui::BeginTable("Table1", 5))
    {
        ImGui::TableNextColumn(); ImGui::Checkbox("ESP", &g.ESP); ImGui::SameLine();
        ImGui::TableNextColumn(); ImGui::Checkbox("Team ESP", &TeamESP);
        ImGui::TableNextColumn(); ImGui::Checkbox("Dummy ESP", &DummyESP);

        ImGui::EndTable();
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::Checkbox("Box", &ESP_Box);
    ImGui::Checkbox("Line", &ESP_Line);
    ImGui::Checkbox("HealthBar (HP + Shield)", &ESP_HealthBar);
    ImGui::Checkbox("Distance", &ESP_Distance);
    ImGui::SliderFloat("Distance (m)", &ESP_MaxDist, 25, 2000);

    ImGui::NewLine();

    ImGui::BulletText("ESP Color");
    ImGui::ColorPicker4("Normal", &ESP_Color.x, ImGuiColorEditFlags_DisplayRGB);

    ImGui::PopItemWidth();
    ImGui::End();
}

// ESP
void render::m_ESP()
{
    // ImGui Window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)g.GameSize.right, (float)g.GameSize.bottom));
    ImGui::Begin("##ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // Local
    uint64_t LocalPlayer = m.Read<uint64_t>(g.BaseAddress + offset::dwLocalPlayer);
    int LocalTeam = m.Read<int>(LocalPlayer + offset::m_iTeamNum);
    int LocalHealth = m.Read<int>(LocalPlayer + offset::m_iHealth);
    Vector3 LocalOrigin = m.Read<Vector3>(LocalPlayer + offset::m_localOrigin);

    // ESP Loop
    for (int i = 0; i < MAX_COUNT; i++)
    {
        uint64_t Entity = GetEntityById(g.BaseAddress, i);
        
        if (Entity == 0 || Entity == LocalPlayer)
            continue;

        // Entity Team
        int EntTeam = m.Read<int>(Entity + offset::m_iTeamNum);

        if (EntTeam == 0)
            continue;
        else if (EntTeam == LocalTeam)
            if (!TeamESP)
                continue;

        // Entity Health
        int EntHealth = m.Read<int>(Entity + offset::m_iHealth);

        if (EntHealth <= 0)
            continue;

        // Entity position
        Vector3 EntityOrigin = m.Read<Vector3>(Entity + offset::m_localOrigin);

        // W2S関連
        uint64_t viewRenderer = m.Read<uint64_t>(g.BaseAddress + offset::OFFSET_Rnder);
        uint64_t tmp = m.Read<uint64_t>(viewRenderer + offset::OFFSET_Matrix);
        MATRIX ViewMatrix = m.Read<MATRIX>(tmp);
        
        // W2S
        Vector2 EntityScreenPos = {};
        WorldToScreen(EntityOrigin, ViewMatrix.matrix , (float)g.GameSize.right, (float)g.GameSize.bottom, EntityScreenPos);

        // ターゲットがレンダリング可能な範囲内にいたら
        if (EntityScreenPos.x != 0.f && EntityScreenPos.y != 0.f)
        {
            // Distance
            float pDist = ((EntityOrigin - LocalOrigin).Length() * 0.01905f);

            // Distance Check
            if (ESP_MaxDist > pDist)
            {
                // Line
                if (ESP_Line)
                    DrawLine(ImVec2((float)g.GameSize.right / 2, (float)g.GameSize.bottom), ImVec2(EntityScreenPos.x, EntityScreenPos.y), ESP_Color, 1);

                // Head ScreenPosision
                Vector3 HeadGamePos = GetEntityBonePosition(Entity, 8, EntityOrigin);
                Vector2 HeadScreenPos = {};
                WorldToScreen(HeadGamePos, ViewMatrix.matrix, (float)g.GameSize.right, (float)g.GameSize.bottom, HeadScreenPos);

                // Box用
                float Height = abs(abs(HeadScreenPos.y) - abs(EntityScreenPos.y));
                float Width = Height / 1.75f;

                // Box
                if (ESP_Box)
                {
                    DrawLine(ImVec2((HeadScreenPos.x + Width / 2), HeadScreenPos.y), ImVec2((HeadScreenPos.x + Width / 3), HeadScreenPos.y), ESP_Color, 1);
                    DrawLine(ImVec2((HeadScreenPos.x - Width / 2), HeadScreenPos.y), ImVec2((HeadScreenPos.x - Width / 3), HeadScreenPos.y), ESP_Color, 1);
                    DrawLine(ImVec2((HeadScreenPos.x + Width / 2), HeadScreenPos.y), ImVec2((HeadScreenPos.x + Width / 2), HeadScreenPos.y + Height / 4), ESP_Color, 1);
                    DrawLine(ImVec2((HeadScreenPos.x - Width / 2), HeadScreenPos.y), ImVec2((HeadScreenPos.x - Width / 2), HeadScreenPos.y + Height / 4), ESP_Color, 1);

                    DrawLine(ImVec2((HeadScreenPos.x + Width / 2), EntityScreenPos.y), ImVec2((HeadScreenPos.x + Width / 2), EntityScreenPos.y - (Height / 4)), ESP_Color, 1);
                    DrawLine(ImVec2((HeadScreenPos.x - Width / 2), EntityScreenPos.y), ImVec2((HeadScreenPos.x - Width / 2), EntityScreenPos.y - (Height / 4)), ESP_Color, 1);
                    DrawLine(ImVec2((HeadScreenPos.x + Width / 2), EntityScreenPos.y), ImVec2((HeadScreenPos.x + Width / 3), EntityScreenPos.y), ESP_Color, 1);
                    DrawLine(ImVec2((HeadScreenPos.x - Width / 2), EntityScreenPos.y), ImVec2((HeadScreenPos.x - Width / 3), EntityScreenPos.y), ESP_Color, 1);
                    
                }

                // Health Bar
                if (ESP_HealthBar)
                {
                    int EntShield    = m.Read<int>(Entity + offset::m_shieldHealth);
                    int EntMaxHealth = m.Read<int>(Entity + offset::m_iMaxHealth);
                    int EntMaxShield = m.Read<int>(Entity + offset::m_shieldHealthMax);

                    ProgressBar((HeadScreenPos.x - (Width / 1.75f) - 3), (HeadScreenPos.y + Height), 2, -Height, EntHealth + EntShield, EntMaxHealth + EntMaxShield);
                }

                // Distance
                if (ESP_Distance)
                {
                    std::string distance = std::to_string((int)pDist) + "m";
                    String(ImVec2(EntityScreenPos.x - 7, EntityScreenPos.y), WHITE, distance.c_str());
                }
            }
        }
        else
        {
            continue;
        }

    }

    ImGui::End();
}

uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent)
{
    uint64_t EntityList = GameBaseAddr + offset::dwEntityList;
    uint64_t BaseEntity = m.Read<uint64_t>(EntityList + (Ent << 5));
    if (!BaseEntity)
        return 0;
    return BaseEntity;
}

Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition)
{
    unsigned long long pBoneArray = m.Read<unsigned long long>(ent + offset::s_BoneMatrix);

    Vector3 EntityHead = Vector3();

    EntityHead.x = m.Read<float>(pBoneArray + 0xCC + (BoneId * 0x30)) + BasePosition.x;
    EntityHead.y = m.Read<float>(pBoneArray + 0xDC + (BoneId * 0x30)) + BasePosition.y;
    EntityHead.z = m.Read<float>(pBoneArray + 0xEC + (BoneId * 0x30)) + BasePosition.z;

    return EntityHead;
}

bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to)
{
    float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

    if (w < 0.01f) return false;

    to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
    to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

    float invw = 1.0f / w;
    to.x *= invw;
    to.y *= invw;

    float x = targetWidth / 2;
    float y = targetHeight / 2;

    x += 0.5 * to.x * targetWidth + 0.5;
    y -= 0.5 * to.y * targetHeight + 0.5;

    to.x = x;
    to.y = y;

    return true;
}