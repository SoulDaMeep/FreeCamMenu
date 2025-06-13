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
		if (!cam || NoMoveWithKeys) return;

		Rotator rot = cam->RotationOffset;
		
		// These are the default rotations of the camera
		// think of these as local default
		rot.Pitch += cam->currentOrientation.rotation.Pitch;
		rot.Yaw += cam->currentOrientation.rotation.Yaw;

		
		Quat q = RotatorToQuat(rot);
		// Rotate the rotation of the camera off of axis
		Vector forward = RotateVectorWithQuat(Axis::Forward, q);
		Vector right = RotateVectorWithQuat(Axis::Right, q);
		//Vector up      = RotateVectorWithQuat(Axis::Up,      q);
		//Vector up    = Vector::Cros(forward, right);

		ImGuiIO& io = ImGui::GetIO();
		// 'W' is 87 so loop until that point
		// 'W' being the largest char as an int
		for (int i = 0; i < 88; ++i) {
			if (!io.KeysDown[i]) continue;
			switch (i) {
				case ' ':
					cam->currentOrientation.focus.Z += 1 * (MovementSpeed / 2);
					break;
				case 16: // shift
					cam->currentOrientation.focus.Z -= 1 * (MovementSpeed / 2);
					break;
				case 'W':
					cam->currentOrientation.focus += forward.getNormalized() * MovementSpeed;
					break;
				case 'A':
					cam->currentOrientation.focus -= right.getNormalized() * MovementSpeed;
					break;
				case 'S':
					cam->currentOrientation.focus -= forward.getNormalized() * MovementSpeed;
					break;
				case 'D':
					cam->currentOrientation.focus += right.getNormalized() * MovementSpeed;
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
		// game thread search
		gameWrapper->Execute([this](GameWrapper* gw)
		{
			camera = std::make_shared<CameraWrapper>(gw->GetCamera());
			if (!camera) return;
			cam = ((MainMenuCamera*)camera->memory_address)->cam;
			// save defaults
			ColorScaleColor = camera->GetColorScale();
			DefaultFocus = cam->currentOrientation.focus;
		});
	}
	if (cam == nullptr) return;

	ImGui::Checkbox("No Move with Keys", &NoMoveWithKeys);
	ImGui::SliderFloat("Movement Sensitivity", &MovementSpeed, 0.01f, 10.f);
	ImGui::SliderFloat("BlendRate", &cam->BlendRate, -10.0f, 10.0f);
	ImGui::SliderFloat("MinPitch", &cam->minPitch, -20000, 20000);
	ImGui::SliderFloat("MaxPitch", &cam->maxPitch, -20000, 20000);
	ImGui::SliderFloat("Distance", &cam->currentOrientation.distance, 0.0f, 5000.0f);
	ImGui::SliderInt("Roll", &cam->currentOrientation.rotation.Roll, -32768, 32768);
	ImGui::SliderFloat("FOV", &cam->currentOrientation.fov, 0.0f, 360.0f);
	ImGui::DragFloat3("FocusPoint", &cam->currentOrientation.focus.X); ImGui::SameLine();
	if (ImGui::Button("Default"))
		cam->currentOrientation.focus = DefaultFocus;

	ImGui::DragFloat3("Car Pos", &CarPos.X);

	if (ImGui::ColorEdit3("ColorScale (tint)", &ColorScaleColor.X))
		camera->SetColorScale(ColorScaleColor * ColorScaleStrength);

	if (ImGui::SliderFloat("ColorScale Strength", &ColorScaleStrength, 0.0f, 10.0f))
		camera->SetColorScale(ColorScaleColor * ColorScaleStrength);
	

}
