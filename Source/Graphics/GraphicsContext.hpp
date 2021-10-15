#pragma once

#include "Utils/PCH.hpp"
#include "Engine/Subsystem.hpp"
#include "Timing/Stopwatch.hpp"
#include "Eventing/Event.hpp"

namespace Ilum
{
class Instance;
class PhysicalDevice;
class Surface;
class LogicalDevice;
class Swapchain;
class CommandBuffer;
class CommandPool;
class DescriptorCache;
class ImGuiContext;

class GraphicsContext : public TSubsystem<GraphicsContext>
{
  public:
	GraphicsContext(Context *context);

	~GraphicsContext() = default;

	const Instance &getInstance() const;

	const PhysicalDevice &getPhysicalDevice() const;

	const Surface &getSurface() const;

	const LogicalDevice &getLogicalDevice() const;

	const Swapchain &getSwapchain() const;

	DescriptorCache &getDescriptorCache();

	const VkPipelineCache &getPipelineCache() const;

	const ref<CommandPool> &getCommandPool(VkQueueFlagBits queue_type = VK_QUEUE_GRAPHICS_BIT, const std::thread::id &thread_id = std::this_thread::get_id());

	uint32_t getFrameIndex() const;

	const VkSemaphore &getPresentCompleteSemaphore() const;

	const VkSemaphore &getRenderCompleteSemaphore() const;

	const CommandBuffer &getCurrentCommandBuffer() const;

  public:
	virtual bool onInitialize() override;

	virtual void onPreTick() override;

	virtual void onTick(float delta_time) override;

	virtual void onPostTick() override;

	virtual void onShutdown() override;

  private:
	void createSwapchain();

	void createCommandBuffer();

  private:
	void newFrame();

	void submitFrame();

  private:
	scope<Instance>       m_instance        = nullptr;
	scope<PhysicalDevice> m_physical_device = nullptr;
	scope<Surface>        m_surface         = nullptr;
	scope<LogicalDevice>  m_logical_device  = nullptr;
	scope<Swapchain>      m_swapchain       = nullptr;

	scope<DescriptorCache> m_descriptor_cache = nullptr;

	// Command pool per thread
	std::unordered_map<std::thread::id, std::unordered_map<VkQueueFlagBits, ref<CommandPool>>> m_command_pools;

	// Present resource
	std::vector<scope<CommandBuffer>> m_command_buffers;
	std::vector<VkSemaphore>          m_present_complete;
	std::vector<VkSemaphore>          m_render_complete;
	std::vector<VkFence>              m_flight_fences;
	uint32_t                          m_current_frame = 0;
	bool                              m_resized       = false;

	VkPipelineCache m_pipeline_cache = VK_NULL_HANDLE;

	Stopwatch m_stopwatch;

	uint64_t m_frame_count = 0;

  public:
	Event<> Swapchain_Rebuild_Event;
};
}        // namespace Ilum