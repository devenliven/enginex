solution "enginex"
    configurations { "Debug", "Release" }
    location "build"
    startproject "engine"
    architecture "x64"
    cppdialect "C++20"
    location "projects"
    debugdir "%{prj.location}../"
    defines { "WIN32", "_WIN32", "WIN32_LEAN_AND_MEAN", "__STDC_FORMAT_MACROS", "NOMINMAX" }
    buildoptions { "/utf-8" }
    
    filter "configurations:Release"
        optimize "full"
    filter "configurations:Debug*"
        optimize "off"
        symbols "on"

    project "engine"
        kind "consoleapp"
        pchheader "pch.h"
        pchsource "src/pch.cpp"
        characterset "mbcs"
        buildoptions { "/Zc:__cplusplus", "/Zc:char8_t-" }
        
        links {
            "spdlog",
            "glad",
            "assimp"
        }

        includedirs {
            "3rdparty/spdlog/include",
            "3rdparty/glm",
            "3rdparty/glad/include",
            "3rdparty/assimp/include",
            "src/"
        }

        files {
            "src/**"
        }

        links { 
            "kernel32", 
            "gdi32",
            "opengl32",
            "user32",
            "shell32",
            "ole32"
        }
        postbuildcommands { "{COPY} %{cfg.buildtarget.relpath} %{prj.location}../" }
        
        filter "configurations:Debug*"
            targetname "engine_d"

    group "3rdparty"
        project "glad"
            kind "staticlib"
            language "c"
            
            files {
                "3rdparty/glad/include/glad/glad.h",
                "3rdparty/glad/include/KHR/khrplatform.h",
                "3rdparty/glad/src/glad.c"
            }
            
            includedirs {
                "3rdparty/glad/include"
            }
            
            filter "configurations:Release"
                optimize "full"
            
            filter "configurations:Debug*"
                optimize "off"
                symbols "on"
        
        project "spdlog"
            kind "staticlib"
            language "c++"
            cppdialect "c++17"
            exceptionhandling "off"
            architecture "x86_64"
            rtti "off"
            
            files {
                "3rdparty/spdlog/include/spdlog/*/*.h",
                "3rdparty/spdlog/src/*.cpp",
            }
            
            removefiles {
                "3rdparty/spdlog/include/spdlog/*/*-inl.h"
            }
            
            includedirs {
                "3rdparty/spdlog/include"
            }
            
            defines { "SPDLOG_COMPILED_LIB", "FMT_UNICODE=0", "_CRT_SECURE_NO_WARNINGS" }
            
            filter "configurations:Release"
                optimize "full"
            
            filter "configurations:Debug*"
                optimize "off"
                symbols "on"
        
        project "assimp"
            kind "staticlib"
            defines { "RAPIDJSON_HAS_STDSTRING" }
            filter "*"

             includedirs {
                "3rdparty/configs",
                "3rdparty/configs/assimp",
                "3rdparty/assimp/include",
                "3rdparty/assimp/code",
                "3rdparty/assimp/contrib",
                "3rdparty/assimp/contrib/rapidjson/include",
                "3rdparty/assimp/contrib/unzip",
                "3rdparty/assimp/contrib/zlib",
                "3rdparty/assimp/contrib/utf8cpp/source",
                "3rdparty/configs/assimp/contrib/zlib",
                "3rdparty/configs/assimp/contrib/",
                "3rdparty/configs/assimp/contrib/earcut-hpp"
            }

            files {
                "3rdparty/assimp/include/assimp/*",
                "3rdparty/assimp/code/AssetLib/FBX/*",
                "3rdparty/assimp/code/AssetLib/glTF/*",
                "3rdparty/assimp/code/AssetLib/glTF2/*",
                "3rdparty/assimp/code/AssetLib/glTFCommon/*",
                "3rdparty/assimp/code/CApi/*",
                "3rdparty/assimp/code/Common/*",
                "3rdparty/assimp/code/Geometry/*",
                "3rdparty/assimp/code/Material/*",
                "3rdparty/assimp/code/Pbrt/*",
                "3rdparty/assimp/code/PostProcessing/*",
                "3rdparty/assimp/contrib/zlib/*.c",
                "3rdparty/configs/assimp/*",
                "3rdparty/configs/*"
            }

            removefiles {
                "3rdparty/assimp/code/AssetLib/FBX/FBXExporter.cpp",
                "3rdparty/assimp/code/AssetLib/FBX/FBXExporter.h",
                "3rdparty/assimp/code/AssetLib/FBX/FBXExportNode.cpp",
                "3rdparty/assimp/code/AssetLib/FBX/FBXExportNode.h",
                "3rdparty/assimp/code/AssetLib/FBX/FBXExportProperty.cpp",
                "3rdparty/assimp/code/AssetLib/FBX/FBXExportProperty.h",
                "3rdparty/assimp/code/AssetLib/glTF/glTFAssetWriter.h",
                "3rdparty/assimp/code/AssetLib/glTF/glTFAssetWriter.inl",
                "3rdparty/assimp/code/AssetLib/glTF/glTFExporter.cpp",
                "3rdparty/assimp/code/AssetLib/glTF/glTFExporter.h",
                "3rdparty/assimp/code/AssetLib/glTF2/glTF2AssetWriter.h",
                "3rdparty/assimp/code/AssetLib/glTF2/glTF2AssetWriter.inl",
                "3rdparty/assimp/code/AssetLib/glTF2/glTF2Exporter.cpp",
                "3rdparty/assimp/code/AssetLib/glTF2/glTF2Exporter.h",
            }
            
            defines {
                "ASSIMP_BUILD_NO_3DS_IMPORTER",
                "ASSIMP_BUILD_NO_3MF_IMPORTER",
                "ASSIMP_BUILD_NO_AC_IMPORTER",
                "ASSIMP_BUILD_NO_AMF_IMPORTER",
                "ASSIMP_BUILD_NO_ASE_IMPORTER",
                "ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
                "ASSIMP_BUILD_NO_B3D_IMPORTER",
                "ASSIMP_BUILD_NO_BLENDER_IMPORTER",
                "ASSIMP_BUILD_NO_BVH_IMPORTER",
                "ASSIMP_BUILD_NO_C4D_IMPORTER",
                "ASSIMP_BUILD_NO_COB_IMPORTER",
                "ASSIMP_BUILD_NO_COLLADA_IMPORTER",
                "ASSIMP_BUILD_NO_CSM_IMPORTER",
                "ASSIMP_BUILD_NO_DXF_IMPORTER",
                -- "ASSIMP_BUILD_NO_FBX_IMPORTER",
                -- "ASSIMP_BUILD_NO_GLTF_IMPORTER",
                -- "ASSIMP_BUILD_NO_GLTF2_IMPORTER",
                "ASSIMP_BUILD_NO_HMP_IMPORTER",
                "ASSIMP_BUILD_NO_IFC_IMPORTER",
                "ASSIMP_BUILD_NO_IQM_IMPORTER",
                "ASSIMP_BUILD_NO_IRR_IMPORTER",
                "ASSIMP_BUILD_NO_LWO_IMPORTER",
                "ASSIMP_BUILD_NO_LWS_IMPORTER",
                "ASSIMP_BUILD_NO_MD3_IMPORTER",
                "ASSIMP_BUILD_NO_MDL_IMPORTER",
                "ASSIMP_BUILD_NO_MD2_IMPORTER",
                "ASSIMP_BUILD_NO_3DS_IMPORTER",
                "ASSIMP_BUILD_NO_USD_IMPORTER",
                "ASSIMP_BUILD_NO_X_IMPORTER",
                "ASSIMP_BUILD_NO_PLY_IMPORTER",
                "ASSIMP_BUILD_NO_OBJ_IMPORTER",
                "ASSIMP_BUILD_NO_SMD_IMPORTER",
                "ASSIMP_BUILD_NO_MDC_IMPORTER",
                "ASSIMP_BUILD_NO_MD5_IMPORTER",
                "ASSIMP_BUILD_NO_STL_IMPORTER",
                "ASSIMP_BUILD_NO_NFF_IMPORTER",
                "ASSIMP_BUILD_NO_RAW_IMPORTER",
                "ASSIMP_BUILD_NO_SIB_IMPORTER",
                "ASSIMP_BUILD_NO_OFF_IMPORTER",
                "ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
                "ASSIMP_BUILD_NO_Q3D_IMPORTER",
                "ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
                "ASSIMP_BUILD_NO_3D_IMPORTER",
                "ASSIMP_BUILD_NO_OGRE_IMPORTER",
                "ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
                "ASSIMP_BUILD_NO_MS3D_IMPORTER",
                "ASSIMP_BUILD_NO_BLEND_IMPORTER",
                "ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
                "ASSIMP_BUILD_NO_NDO_IMPORTER",
                "ASSIMP_BUILD_NO_XGL_IMPORTER",
                "ASSIMP_BUILD_NO_X3D_IMPORTER",
                "ASSIMP_BUILD_NO_MMD_IMPORTER",
                "ASSIMP_BUILD_NO_M3D_IMPORTER",

                -- exporters
                "ASSIMP_BUILD_NO_FBX_EXPORTER",
                "ASSIMP_BUILD_NO_GLTF_EXPORTER",
                "ASSIMP_BUILD_NO_GLTF2_EXPORTER",
            }
