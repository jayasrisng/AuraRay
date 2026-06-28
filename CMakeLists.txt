cmake_minimum_required(VERSION 3.20)

project(AuraRay VERSION 0.1.0 LANGUAGES CXX)

set(AURARAY_RENDERER_SOURCES
    src/core/Camera.cpp
    src/core/Random.cpp
    src/core/Vec3.cpp
    src/geometry/Hittable.cpp
    src/geometry/HittableList.cpp
    src/geometry/Sphere.cpp
    src/io/ImageWriter.cpp
    src/io/MetadataWriter.cpp
    src/materials/Dielectric.cpp
    src/materials/Lambertian.cpp
    src/materials/Metal.cpp
    src/render/Foveation.cpp
    src/render/Renderer.cpp
    src/render/Scene.cpp
)

add_library(auraray_renderer STATIC ${AURARAY_RENDERER_SOURCES})
target_include_directories(auraray_renderer PUBLIC src)
target_compile_features(auraray_renderer PUBLIC cxx_std_17)
set_target_properties(auraray_renderer PROPERTIES CXX_EXTENSIONS OFF)

add_executable(auraray src/main.cpp)
target_link_libraries(auraray PRIVATE auraray_renderer)

if(MSVC)
    target_compile_options(auraray_renderer PRIVATE /W4)
    target_compile_options(auraray PRIVATE /W4)
else()
    target_compile_options(auraray_renderer PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(auraray PRIVATE -Wall -Wextra -pedantic)
endif()

include(CTest)
if(BUILD_TESTING AND UNIX)
    add_test(
        NAME render_reproducibility
        COMMAND sh ${CMAKE_SOURCE_DIR}/scripts/verify_render_reproducibility.sh
                $<TARGET_FILE:auraray>
    )
    set_tests_properties(render_reproducibility PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
