SetTarget(
    MODE STATIC
    TARGET_NAME Ilum
    DEFINE
        PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}/"
        NOMINMAX
        ENTT_USE_ATOMIC
    INC
        ${PROJECT_SOURCE_DIR}/Source/Ilum
    LIB
        SDL2::SDL2
        spdlog
        volk
        glslang
        SPIRV
        spirv-cross-glsl
        vulkan
        stb
        vma
        assimp::assimp
        entt
        imgui
        yaml-cpp
        glm
        ImGuizmo
        imgui-node-editor
        ImFileDialog
)