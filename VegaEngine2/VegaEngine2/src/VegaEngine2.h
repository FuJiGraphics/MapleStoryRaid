#pragma once

// Standards
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <functional>
#include <ImguiSFML/imgui-SFML.h>
#include <imgui.h>

// Data Types
#include "DataType/DataType.h"

// Utility
#include "Utility/Utility.h"

// Events
#include "Events/Events.h"

// Input
#include "Managers/Managers.h"

// Export VegaEngine
#include "Core/Defines.h"
#include "Core/System.h"
#include "Core/Layer/Layer.h"
#include "Core/Layer/LayerPool.h"

// Renderer
#include "Renderer/Renderer2D.h"
#include "Renderer/FrameBuffer.h"

// Scene
#include "Core/Scene/Components.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/VegaScript.h"
#include "Core/Scene/EditorCamera.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/ScriptCore.h"
#include "Core/Scene/SceneManager.h"

#include "Animation/AnimationClip.h"
#include "Animation/Animator.h"

/* Entry Point */
#include "EntryPoint.h"
#include "AttachScripts.h"