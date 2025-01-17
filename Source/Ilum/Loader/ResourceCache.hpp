#pragma once

#include "Graphics/Image/Image.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderReflection.hpp"

namespace Ilum
{
class ResourceCache
{
  public:
	ResourceCache() = default;

	~ResourceCache() = default;

	ImageReference loadImage(const std::string &filepath);

	void loadImageAsync(const std::string &filepath);

	void removeImage(const std::string &filepath);

	bool hasImage(const std::string &filepath) const;

	const std::unordered_map<std::string, size_t> &getImages();

	void updateImageReferences();

	const std::vector<ImageReference> &getImageReferences() const;

	uint32_t imageID(const std::string &filepath);

	void clearImages();

	ModelReference loadModel(const std::string &name);

	void loadModelAsync(const std::string &filepath);

	void removeModel(const std::string &filepath);

	bool hasModel(const std::string &filepath);

	const std::unordered_map<std::string, size_t> &getModels();

	void clearModels();

	void clear();

	void flush();

  private:
	// Cache image
	std::vector<Image>                      m_image_cache;
	std::unordered_map<std::string, size_t> m_image_map;
	std::vector<std::string>                m_deprecated_image;
	std::unordered_set<std::string>         m_new_image;
	std::vector<ImageReference>             m_image_references;
	std::unordered_map<std::string, std::future<Image>> m_image_futures;

	// Cache model
	std::vector<Model>                      m_model_cache;
	std::unordered_map<std::string, size_t> m_model_map;
	std::vector<std::string>                m_deprecated_model;
	std::unordered_set<std::string>         m_new_model;
	std::unordered_map<std::string, std::future<Model>> m_model_futures;

	std::mutex m_image_mutex;
	std::mutex m_model_mutex;
};
}        // namespace Ilum