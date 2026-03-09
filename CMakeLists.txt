cmake_minimum_required(VERSION 3.20)
project(GPUtopia2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies via vcpkg ---
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenCL REQUIRED)
find_package(GLEW REQUIRED)
find_package(tinyfiledialogs CONFIG REQUIRED)
find_package(Stb REQUIRED)

set(SOURCES
    GPUtopia2/main.cpp
    GPUtopia2/display.cpp
    GPUtopia2/cl_core.cpp
    GPUtopia2/cl_async.cpp
    GPUtopia2/cl_fractal.cpp
    GPUtopia2/cl_handler.cpp
    GPUtopia2/cl_erro_handling.cpp
    GPUtopia2/cl_error_window.cpp
    GPUtopia2/clipboard.cpp
    GPUtopia2/color.cpp
    GPUtopia2/cubic_spline.cpp
    GPUtopia2/explorer.cpp
    GPUtopia2/fileOperations.cpp
    GPUtopia2/flameRenderSettings.cpp
    GPUtopia2/formulaSettings.cpp
    GPUtopia2/fractal_navigation.cpp
    GPUtopia2/gradient.cpp
    GPUtopia2/gradientWindow.cpp
    GPUtopia2/history.cpp
    GPUtopia2/imageSettings.cpp
    GPUtopia2/info.cpp
    GPUtopia2/insideSettings.cpp
    GPUtopia2/load_shaders.cpp
    GPUtopia2/outsideSettings.cpp
    GPUtopia2/sampling.cpp
    GPUtopia2/selectFile.cpp
    GPUtopia2/test_gradients.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    GPUtopia2
    ${Stb_INCLUDE_DIR}
)

# GLEW includes GL/glu.h by default which may not be installed; we don't use GLU.
target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_NO_GLU)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    imgui::imgui
    OpenCL::OpenCL
    GLEW::GLEW
    tinyfiledialogs::tinyfiledialogs
)

# Copy runtime assets to the build directory
set(ASSET_FILES
    GPUtopia2/segoeui.ttf
    GPUtopia2/DroidSansMono.ttf
    GPUtopia2/vertex_shader.glsl
    GPUtopia2/fragment_shader.glsl
)
foreach(ASSET ${ASSET_FILES})
    get_filename_component(ASSET_NAME ${ASSET} NAME)
    configure_file(${ASSET} ${CMAKE_CURRENT_BINARY_DIR}/${ASSET_NAME} COPYONLY)
endforeach()

# Copy clFragments directory to build directory
file(COPY GPUtopia2/clFragments DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
