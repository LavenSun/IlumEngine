#pragma once

#include "Utils/PCH.hpp"

#include "Engine/Subsystem.hpp"

#include "Graphics/Image/Image.hpp"

namespace Ilum
{
class CommandBuffer;
class Sampler;

class ImGuiContext
{
  public:
	enum class StyleType
	{
		DarkMode
	};

  public:
	ImGuiContext();

	~ImGuiContext() = default;

	static void initialize();

	static void destroy();

	static void createResouce();

	static void releaseResource();

	static void begin();

	static void render(const CommandBuffer &command_buffer);

	static void end();

	static void beginDockingSpace();

	static void endDockingSpace();

	static void *textureID(const Image &image, const Sampler &sampler);

	static void *textureID(const VkImageView &view, const Sampler &sampler);

	static void flush();

	bool enable() const;

  private:
	static void setDarkMode();

  private:
	static scope<ImGuiContext> s_instance;

	VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;

	std::unordered_map<size_t, VkDescriptorSet> m_texture_id_mapping;

	// File dialog
	std::unordered_map<VkDescriptorSet, Image> m_filedialog_image_cache;
	std::vector<VkDescriptorSet>               m_deprecated_descriptor_sets;

	static bool s_enable;
};
}        // namespace Ilum