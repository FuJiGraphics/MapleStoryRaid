#pragma warning(disable : 4006)
#pragma once

// #define FZLOG_DEBUG_MODE_ENABLED

// Standards
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <functional>
#include <list>
#include <cassert>
#include <queue>
#include <map>
#include <mutex>
#include <thread>

// Box2D
#include <box2d/box2d.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

// Data Types
#include "DataType/DataType.h"

// SFML
#include "entt/entt.hpp"
#include "Platform/SFML/VegaSFML.h"

// Utility
#include "Utility/Utility.h"

// Manager
#include "Managers/Managers.h"

// Events
#include "Events/Events.h"

// Cores
#include "Core/Defines.h"
#include "Core/Window/Window.h"
#include "Core/Layer/Layer.h"
#include "Core/Layer/LayerStack.h"
#include "Core/Layer/LayerPool.h"
#include "Core/System.h"
#include "Core/Scene/EditorCamera.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/EntitySerializer.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Scene/VegaScript.h"
#include "Core/Scene/CollisionHandler.h"
#include "Core/Scene/Components.h"

// Renderer
#include "Renderer/Camera.h"
#include "Renderer/Renderer2D.h"

// Animation
#include "Animation/AnimationClip.h"
#include "Animation/Animator.h"

// Script
#include "AttachScripts.h"

