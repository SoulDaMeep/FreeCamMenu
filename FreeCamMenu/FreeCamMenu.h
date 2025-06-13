#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

namespace Axis
{
	const Vector Right = Vector(0.0f, 1.0f, 0.0f);
	const Vector Forward = Vector(1.0, 0.0f, 0.0f);
	const Vector Up = Vector(0.0f, 0.0f, 1.0f);
};

struct CameraStateX
{
	unsigned char _PAD1[0xb0];
	float BlendRate;
	float minPitch;
	float maxPitch;
	CameraOrientation currentOrientation;
	float CurrentSideOffset;
	Rotator RotationOffset;
};

struct MainMenuCamera
{
	unsigned char _PAD1[0x6e0];
	CameraStateX* cam;
};

class FreeCamMenu: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase 
{

	void onLoad() override;
	void RenderSettings() override; 

public:	
	// Cameras
	std::shared_ptr<CameraWrapper> camera = nullptr;
	CameraStateX* cam = nullptr;
public:
	// Variables
	bool NoMoveWithKeys = false;
	float MovementSpeed = 1.0f;
	float ColorScaleStrength = 1.0f;
	Vector ColorScaleColor;
	Vector DefaultFocus;
	Vector CarPos;
};
