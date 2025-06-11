#include "pch.h"
#include "FreeCamMenu.h"
#include <Windows.h>

BAKKESMOD_PLUGIN(FreeCamMenu, "Free Cam in main menu ", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
void FreeCamMenu::onLoad()
{
	_globalCvarManager = cvarManager;
	gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick",
		[this](...)
	{
		if (!cam) return;

		Rotator rot = cam->RotationOffset;
		rot.Pitch += -385;
		rot.Yaw += 25752;
		float yaw_rads = rot.Yaw * CONST_UnrRotToRad;
		float pitch_rads = rot.Pitch * CONST_UnrRotToRad;
		float x = cos(yaw_rads);
		float y = sin(yaw_rads);
		float z = sin(pitch_rads);
		Vector forward = Vector(x, y, z);
		Vector right = RotateVectorWithQuat(forward, RotatorToQuat(VectorToRotator(Axis::Right)));
		right = RotateVectorWithQuat(right, RotatorToQuat(VectorToRotator(Axis::Right)));

		Vector up = Vector::cross(forward, right);
		ImGuiIO& io = ImGui::GetIO();
		for (int i = 0; i < 87; ++i) {
			if (!io.KeysDown[i]) continue;
			switch (i) {
				case ' ':
					cam->currentOrientation.focus.Z += 1;
					break;
				case 16: // shift
					cam->currentOrientation.focus.Z -= 1;
					break;
				case 'W':
					cam->currentOrientation.focus += forward.getNormalized() * 2;
					break;
				case 'A':
					cam->currentOrientation.focus -= right.getNormalized() * 2;
					break;
				case 'S':
					cam->currentOrientation.focus -= forward.getNormalized() * 2;
					break;
				case 'D':
					cam->currentOrientation.focus += right.getNormalized() * 2;
					break;
			}
		}
	});
}

void FreeCamMenu::RenderSettings()
{

	if (gameWrapper->IsInGame() || gameWrapper->IsInFreeplay())
	{
		if (cam != nullptr)
			cam = nullptr;
		return;
	}

	ImGui::Text("%s", cam != nullptr ? "Found Camera..." : "Cant find camera...");
	if (ImGui::Button("Find Camera"))
	{
		gameWrapper->Execute([this](GameWrapper* gw)
		{
			camera = std::make_shared<CameraWrapper>(gw->GetCamera());
			if (!camera) return;
			cam = ((MainMenuCamera*)camera->memory_address)->cam;
			ColorScaleColor = camera->GetColorScale();
			DefaultFocus = cam->currentOrientation.focus;
		});
	}
	if (cam == nullptr) return;

	ImGui::SliderFloat("BlendRate", &cam->BlendRate, -10.0f, 10.0f);
	ImGui::SliderFloat("MinPitch", &cam->minPitch, -20000, 20000);
	ImGui::SliderFloat("MaxPitch", &cam->maxPitch, -20000, 20000);
	ImGui::SliderFloat("Distance", &cam->currentOrientation.distance, 0.0f, 5000.0f);
	ImGui::SliderInt("Roll", &cam->currentOrientation.rotation.Roll, -32768, 32768);
	ImGui::SliderFloat("FOV", &cam->currentOrientation.fov, 0.0f, 360.0f);
	ImGui::DragFloat3("FocusPoint", &cam->currentOrientation.focus.X); ImGui::SameLine();
	if (ImGui::Button("Default"))
	{
		cam->currentOrientation.focus = DefaultFocus;
	}
	if (ImGui::ColorEdit3("ColorScale (tint)", &ColorScaleColor.X))
	{
		camera->SetColorScale(ColorScaleColor * ColorScaleStrength);
	}
	if (ImGui::SliderFloat("ColorScale Strength", &ColorScaleStrength, 0.0f, 10.0f))
	{
		camera->SetColorScale(ColorScaleColor * ColorScaleStrength);
	}

}
