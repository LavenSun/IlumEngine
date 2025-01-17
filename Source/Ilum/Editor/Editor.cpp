#include "Editor.hpp"

#include <imgui.h>

#include "Device/Window.hpp"

#include "ImGui/ImGuiContext.hpp"

#include "File/FileSystem.hpp"

#include "Renderer/RenderGraph/RenderGraph.hpp"
#include "Renderer/RenderPass/ImGuiPass.hpp"
#include "Renderer/Renderer.hpp"

#include "Panels/AssetBrowser.hpp"
#include "Panels/Console.hpp"
#include "Panels/Hierarchy.hpp"
#include "Panels/Inspector.hpp"
#include "Panels/ProfilerMonitor.hpp"
#include "Panels/RenderGraphViewer.hpp"
#include "Panels/RenderSetting.hpp"
#include "Panels/SceneView.hpp"

#include "Scene/Component/DirectionalLight.hpp"
#include "Scene/Component/Light.hpp"
#include "Scene/Component/MeshRenderer.hpp"
#include "Scene/Component/PointLight.hpp"
#include "Scene/Component/SpotLight.hpp"
#include "Scene/SceneSerializer.hpp"

#include "ImFileDialog.h"

namespace Ilum
{
Editor::Editor(Context *context) :
    TSubsystem<Editor>(context)
{
}

bool Editor::onInitialize()
{
	if (!Renderer::instance()->hasImGui())
	{
		Renderer::instance()->setImGui(true);
		Renderer::instance()->rebuild();
	}

	ImGuiContext::initialize();

	m_panels.emplace_back(createScope<panel::RenderGraphViewer>());
	m_panels.emplace_back(createScope<panel::Inspector>());
	m_panels.emplace_back(createScope<panel::Hierarchy>());
	m_panels.emplace_back(createScope<panel::AssetBrowser>());
	m_panels.emplace_back(createScope<panel::SceneView>());
	m_panels.emplace_back(createScope<panel::Console>());
	m_panels.emplace_back(createScope<panel::RenderSetting>());
	m_panels.emplace_back(createScope<panel::ProfilerMonitor>());

	return true;
}

void Editor::onPreTick()
{
	ImGuiContext::begin();
}

void Editor::onTick(float delta_time)
{
	if (!Renderer::instance()->hasImGui())
	{
		return;
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				Scene::instance()->clear();
				Renderer::instance()->getResourceCache().clear();
				m_scene_path.clear();
				LOG_INFO("Create new scene");
			}

			if (ImGui::MenuItem("Open Scene"))
			{
				ifd::FileDialog::Instance().Open("OpenSceneDialog", "Open Scene", "Scene file (*.scene){.scene}");
			}

			if (ImGui::MenuItem("Save Scene"))
			{
				if (m_scene_path.empty())
				{
					ifd::FileDialog::Instance().Save("SaveSceneDialog", "Save Scene", "Scene file (*.scene){.scene}");
				}
				else
				{
					SceneSerializer serializer;
					serializer.serialize(m_scene_path.c_str());
					Scene::instance()->name = FileSystem::getFileName(m_scene_path, false);
					LOG_INFO("Save scene to {}", m_scene_path);
				}
			}

			if (ImGui::MenuItem("Save as ..."))
			{
				ifd::FileDialog::Instance().Save("SaveSceneDialog", "Save Scene", "Scene file (*.scene){.scene}");
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Panel"))
		{
			for (auto &panel : m_panels)
			{
				ImGui::MenuItem(panel->name().c_str(), nullptr, &panel->active);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Entity"))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				auto entity     = Scene::instance()->createEntity();
				m_select_entity = entity;
			}
			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					auto entity     = Scene::instance()->createEntity("Directional Light");
					m_select_entity = entity;

					entity.addComponent<cmpt::Light>().type = cmpt::LightType::Directional;
				}
				if (ImGui::MenuItem("Point Light"))
				{
					auto entity     = Scene::instance()->createEntity("Point Light");
					m_select_entity = entity;

					entity.addComponent<cmpt::Light>().type = cmpt::LightType::Point;
				}
				if (ImGui::MenuItem("Spot Light"))
				{
					auto entity     = Scene::instance()->createEntity("Spot Light");
					m_select_entity = entity;

					entity.addComponent<cmpt::Light>().type = cmpt::LightType::Spot;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Model"))
			{
				if (ImGui::MenuItem("Plane"))
				{
					auto entity                                     = Scene::instance()->createEntity("Plane");
					entity.addComponent<cmpt::MeshRenderer>().model = std::string(PROJECT_SOURCE_DIR) + "Asset/Model/plane.obj";
					Renderer::instance()->getResourceCache().loadModelAsync(std::string(PROJECT_SOURCE_DIR) + "Asset/Model/plane.obj");
					m_select_entity = entity;
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ifd::FileDialog::Instance().IsDone("OpenSceneDialog"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				m_scene_path = ifd::FileDialog::Instance().GetResult().u8string();
				SceneSerializer serializer;
				serializer.deserialize(m_scene_path.c_str());
			}
			ifd::FileDialog::Instance().Close();
		}

		if (ifd::FileDialog::Instance().IsDone("SaveSceneDialog"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				m_scene_path = ifd::FileDialog::Instance().GetResult().u8string();
				Scene::instance()->name = FileSystem::getFileName(m_scene_path, false);
				SceneSerializer serializer;
				serializer.serialize(m_scene_path.c_str());
				LOG_INFO("Save scene to {}", m_scene_path);
			}
			ifd::FileDialog::Instance().Close();
		}

		ImGui::EndMainMenuBar();
	}

	for (auto &panel : m_panels)
	{
		if (panel->active)
		{
			panel->draw(delta_time);
		}
	}
}

void Editor::onPostTick()
{
	ImGuiContext::end();
}

void Editor::onShutdown()
{
	ImGuiContext::destroy();

	m_panels.clear();
}

void Editor::select(Entity entity)
{
	m_select_entity = entity;
}

Entity Editor::getSelect()
{
	return m_select_entity;
}

}        // namespace Ilum