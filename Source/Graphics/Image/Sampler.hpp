#pragma once

#include "Utils/PCH.hpp"

namespace Ilum
{
class Sampler
{
  public:
	Sampler() = default;

	Sampler(VkFilter min_filter, VkFilter mag_filter, VkSamplerAddressMode address_mode, VkFilter mip_filter);

	~Sampler();

	const VkSampler &getSampler() const;

	operator const VkSampler &() const;

  private:
	VkSampler m_handle = VK_NULL_HANDLE;
};

using SamplerRefernece = std::reference_wrapper<VkSampler>;
}        // namespace Ilum