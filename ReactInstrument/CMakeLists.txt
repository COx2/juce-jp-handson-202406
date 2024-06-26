project(ReactInstrument VERSION 0.1.0)

set (BaseTargetName ReactInstrument)

juce_add_plugin("${BaseTargetName}"
    # VERSION ...
    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME "COx2"
    NEEDS_WEBVIEW2 TRUE         # On Windows, JUCE may or may not need to link to WebView2 depending on the compile definitions...
    NEEDS_WEB_BROWSER TRUE      # On Linux, JUCE may or may not need to link to Webkit depending on the compile definitions...
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Cox2
    PLUGIN_CODE Rcti
    FORMATS Standalone VST3 AU #AUv3 #LV2
    LV2URI "urn:cox2:ReactInstrument"
    PRODUCT_NAME "ReactInstrument")

clap_juce_extensions_plugin(TARGET "${BaseTargetName}"
    CLAP_ID "com.cox2.reactinstrument"
    CLAP_FEATURES instrument "sampler" basses leads pads)

juce_add_binary_data("${BaseTargetName}_WebViewBundle"
    HEADER_NAME "WebViewBundleData.h"
    NAMESPACE "WebView"
    SOURCES
        WebView/WebViewBundle.zip
    )

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties("${BaseTargetName}_WebViewBundle"
    PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE)

target_sources(${BaseTargetName}
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/JuceDemoSynthesizer.cpp
        Source/JuceDemoSynthesizer.h
        Source/SinglePageBrowser.cpp
        Source/SinglePageBrowser.h
        Source/WebViewBackendComponent.cpp
        Source/WebViewBackendComponent.h
        )

target_compile_definitions(${BaseTargetName}
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# Use PUBLIC to effect linked target like VST3, Standalone, CLAP, ...
target_compile_features(${BaseTargetName}
    PUBLIC
        cxx_std_17)

target_link_libraries(${BaseTargetName}
    PRIVATE
        "${BaseTargetName}_WebViewBundle"
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_audio_devices
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

message(STATUS "[${BaseTargetName}] Binary directory: ${CMAKE_CURRENT_BINARY_DIR}")

# Add cmake install task for this project.
if(TARGET ${BaseTargetName}_VST3)
    get_target_property(VST3_PRODUCT_FILE ${BaseTargetName}_VST3 JUCE_PLUGIN_ARTEFACT_FILE)
    
    message("[${BaseTargetName}] VST3_PRODUCT_FILE: ${VST3_PRODUCT_FILE}")

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "/Library/Audio/Plug-Ins/VST3"
                COMPONENT ${BaseTargetName}_VST3)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "C:/Program Files/Common Files/VST3"
                COMPONENT ${BaseTargetName}_VST3)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "/usr/lib/vst3"
                COMPONENT ${BaseTargetName}_VST3)
    endif()

endif()

if(TARGET ${BaseTargetName}_AU)
    get_target_property(AU_PRODUCT_FILE ${BaseTargetName}_AU JUCE_PLUGIN_ARTEFACT_FILE)

    message("[${BaseTargetName}] AU_PRODUCT_FILE: ${AU_PRODUCT_FILE}")

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        install(FILES "${AU_PRODUCT_FILE}"
                DESTINATION "/Library/Audio/Plug-Ins/Components"
                COMPONENT ${BaseTargetName}_AU)
    endif()

endif()

if(TARGET ${BaseTargetName}_CLAP)
    get_target_property(CLAP_PRODUCT_FILE ${BaseTargetName}_CLAP JUCE_PLUGIN_ARTEFACT_FILE)

    message("[${BaseTargetName}] CLAP_PRODUCT_FILE: ${CLAP_PRODUCT_FILE}")

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        install(FILES "${CLAP_PRODUCT_FILE}"
                DESTINATION "/Library/Audio/Plug-Ins/CLAP"
                COMPONENT ${BaseTargetName}_CLAP)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        install(FILES "${CLAP_PRODUCT_FILE}"
                DESTINATION "C:/Program Files/Common Files/CLAP"
                COMPONENT ${BaseTargetName}_CLAP)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        install(FILES "${CLAP_PRODUCT_FILE}"
                DESTINATION "/usr/lib/clap"
                COMPONENT ${BaseTargetName}_CLAP)
    endif()

endif()
