#include "GeometryPass.hpp"

#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Vertex.hpp"

#include "Renderer/Renderer.hpp"

#include "Scene/Component/MeshRenderer.hpp"
#include "Scene/Component/Tag.hpp"
#include "Scene/Component/Transform.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"

#include "Material/DisneyPBR.h"

#include <glm/gtc/type_ptr.hpp>

namespace Ilum::pass
{
void GeometryPass::setupPipeline(PipelineState &state)
{
	state.shader.load(std::string(PROJECT_SOURCE_DIR) + "Asset/Shader/GLSL/geometry.vert", VK_SHADER_STAGE_VERTEX_BIT, Shader::Type::GLSL);
	state.shader.load(std::string(PROJECT_SOURCE_DIR) + "Asset/Shader/GLSL/geometry.frag", VK_SHADER_STAGE_FRAGMENT_BIT, Shader::Type::GLSL);

	state.dynamic_state.dynamic_states = {
	    VK_DYNAMIC_STATE_VIEWPORT,
	    VK_DYNAMIC_STATE_SCISSOR};

	state.vertex_input_state.attribute_descriptions = {
	    VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
	    VkVertexInputAttributeDescription{1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texcoord)},
	    VkVertexInputAttributeDescription{2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
	    VkVertexInputAttributeDescription{3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent)},
	    VkVertexInputAttributeDescription{4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, bitangent)}};

	state.vertex_input_state.binding_descriptions = {
	    VkVertexInputBindingDescription{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};

	state.color_blend_attachment_states.resize(6);

	// Disable blending
	for (auto& color_blend_attachment_state : state.color_blend_attachment_states)
	{
		color_blend_attachment_state.blend_enable = false;
	}

	state.descriptor_bindings.bind(0, 0, "mainCamera", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	state.descriptor_bindings.bind(0, 1, "textureArray", Renderer::instance()->getSampler(Renderer::SamplerType::Trilinear_Clamp), ImageViewType::Native, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

	state.declareAttachment("gbuffer - albedo", VK_FORMAT_R8G8B8A8_UNORM, Renderer::instance()->getRenderTargetExtent().width, Renderer::instance()->getRenderTargetExtent().height);
	state.declareAttachment("gbuffer - normal", VK_FORMAT_R32G32B32A32_SFLOAT, Renderer::instance()->getRenderTargetExtent().width, Renderer::instance()->getRenderTargetExtent().height);
	state.declareAttachment("gbuffer - position", VK_FORMAT_R32G32B32A32_SFLOAT, Renderer::instance()->getRenderTargetExtent().width, Renderer::instance()->getRenderTargetExtent().height);
	state.declareAttachment("gbuffer - depth", VK_FORMAT_R32G32B32A32_SFLOAT, Renderer::instance()->getRenderTargetExtent().width, Renderer::instance()->getRenderTargetExtent().height);
	state.declareAttachment("gbuffer - metallic", VK_FORMAT_R32G32B32A32_SFLOAT, Renderer::instance()->getRenderTargetExtent().width, Renderer::instance()->getRenderTargetExtent().height);
	state.declareAttachment("gbuffer - roughness", VK_FORMAT_R32G32B32A32_SFLOAT, Renderer::instance()->getRenderTargetExtent().width, Renderer::instance()->getRenderTargetExtent().height);
	state.declareAttachment("depth_stencil", VK_FORMAT_D32_SFLOAT_S8_UINT, Renderer::instance()->getRenderTargetExtent().width, Renderer::instance()->getRenderTargetExtent().height);

	state.addOutputAttachment("gbuffer - albedo", AttachmentState::Clear_Color);
	state.addOutputAttachment("gbuffer - normal", AttachmentState::Clear_Color);
	state.addOutputAttachment("gbuffer - position", AttachmentState::Clear_Color);
	state.addOutputAttachment("gbuffer - depth", AttachmentState::Clear_Color);
	state.addOutputAttachment("gbuffer - metallic", AttachmentState::Clear_Color);
	state.addOutputAttachment("gbuffer - roughness", AttachmentState::Clear_Color);
	state.addOutputAttachment("depth_stencil", VkClearDepthStencilValue{1.f, 0u});
}

void GeometryPass::resolveResources(ResolveState &resolve)
{
	resolve.resolve("textureArray", Renderer::instance()->getResourceCache().getImageReferences());
	resolve.resolve("mainCamera", Renderer::instance()->getBuffer(Renderer::BufferType::MainCamera));
}

void GeometryPass::render(RenderPassState &state)
{
	auto &cmd_buffer = state.command_buffer;

	VkRenderPassBeginInfo begin_info = {};
	begin_info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	begin_info.renderPass            = state.pass.render_pass;
	begin_info.renderArea            = state.pass.render_area;
	begin_info.framebuffer           = state.pass.frame_buffer;
	begin_info.clearValueCount       = static_cast<uint32_t>(state.pass.clear_values.size());
	begin_info.pClearValues          = state.pass.clear_values.data();

	vkCmdBeginRenderPass(cmd_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmd_buffer, state.pass.bind_point, state.pass.pipeline);

	for (auto &descriptor_set : state.pass.descriptor_sets)
	{
		vkCmdBindDescriptorSets(cmd_buffer, state.pass.bind_point, state.pass.pipeline_layout, descriptor_set.index(), 1, &descriptor_set.getDescriptorSet(), 0, nullptr);
	}

	auto &extent = Renderer::instance()->getRenderTargetExtent();

	VkViewport viewport = {0, static_cast<float>(extent.height), static_cast<float>(extent.width), -static_cast<float>(extent.height), 0, 1};
	VkRect2D   scissor  = {0, 0, extent.width, extent.height};

	vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);
	vkCmdSetScissor(cmd_buffer, 0, 1, &scissor);

	Scene::instance()->getRegistry().each([](entt::entity) {});

	const auto group = Scene::instance()->getRegistry().group<>(entt::get<cmpt::MeshRenderer, cmpt::Transform, cmpt::Tag>);

	group.each([&](const cmpt::MeshRenderer &mesh_renderer, const cmpt::Transform &transform, const cmpt::Tag &tag) {
		if (Renderer::instance()->getResourceCache().hasModel(mesh_renderer.model) && tag.active)
		{
			auto &model = Renderer::instance()->getResourceCache().loadModel(mesh_renderer.model);

			VkDeviceSize offsets[1] = {0};
			vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &model.get().getVertexBuffer().get().getBuffer(), offsets);
			vkCmdBindIndexBuffer(cmd_buffer, model.get().getIndexBuffer().get().getBuffer(), 0, VK_INDEX_TYPE_UINT32);

			// Model transform push constants
			vkCmdPushConstants(cmd_buffer, state.pass.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), glm::value_ptr(transform.world_transform));

			for (uint32_t i = 0; i < model.get().getSubMeshes().size(); i++)
			{
				if (mesh_renderer.materials[i] && mesh_renderer.materials[i]->type() == typeid(material::DisneyPBR))
				{
					auto *   material = static_cast<material::DisneyPBR *>(mesh_renderer.materials[i].get());

					struct
					{
						float displacement_height;
						uint32_t displacement_map;
					}displacement;

					displacement.displacement_height = material->displacement_height;
					displacement.displacement_map    = Renderer::instance()->getResourceCache().imageID(material->displacement_map);

					vkCmdPushConstants(cmd_buffer, state.pass.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 64, sizeof(displacement), &displacement);

					struct
					{
						glm::vec4 base_color;
						float metallic_factor;
						float roughness_factor;

						uint32_t albedo_map;
						uint32_t normal_map;
						uint32_t metallic_map;
						uint32_t roughness_map;
					} material_data;

					material_data.base_color = material->base_color;
					material_data.metallic_factor = material->metallic_factor;
					material_data.roughness_factor = material->roughness_factor;
					material_data.albedo_map          = Renderer::instance()->getResourceCache().imageID(material->albedo_map);
					material_data.normal_map          = Renderer::instance()->getResourceCache().imageID(material->normal_map);
					material_data.metallic_map        = Renderer::instance()->getResourceCache().imageID(material->metallic_map);
					material_data.roughness_map       = Renderer::instance()->getResourceCache().imageID(material->roughness_map);

					vkCmdPushConstants(cmd_buffer, state.pass.pipeline_layout, VK_SHADER_STAGE_FRAGMENT_BIT, 80, sizeof(material_data), &material_data);

					const auto &submesh = model.get().getSubMeshes()[i];
					vkCmdDrawIndexed(cmd_buffer, submesh.getIndexCount(), 1, submesh.getIndexOffset(), 0, 0);
				}
			}
		}
	});

	vkCmdEndRenderPass(cmd_buffer);
}
}        // namespace Ilum::pass