cmake_minimum_required(VERSION 3.22)

project(SimpleParametricEq VERSION 0.1.0)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/SimpleParametricEq")

juce_add_plugin(
  ${PROJECT_NAME}
  COMPANY_NAME
  "Nirioonossian"
  IS_SYNTH
  FALSE 
  NEEDS_MIDI_INPUT
  FALSE
  NEEDS_MIDI_OUTPUT
  FALSE 
  PLUGIN_MANUFACTURER_CODE
  Niro
  PLUGIN_CODE
  Speq
  FORMATS
  Standalone
  VST3
  AU
  PRODUCT_NAME
  "Simple Parametric EQ"
  COPY_PLUGIN_AFTER_BUILD
  TRUE
)

set(SOURCE_FILES source/PluginEditor.cpp source/PluginProcessor.cpp
source/ParametricEq.cpp source/Parameters.cpp)

set(HEADER_FILES ${INCLUDE_DIR}/PluginEditor.h ${INCLUDE_DIR}/PluginProcessor.h
${INCLUDE_DIR}/filters/BiquadFilter.h ${INCLUDE_DIR}/filters/PeakFilter.h ${INCLUDE_DIR}/filters/LowShelfFilter.h
${INCLUDE_DIR}/filters/FilterParameters.h ${INCLUDE_DIR}/ParametricEq.h ${INCLUDE_DIR}/Parameters.h)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries_system(${PROJECT_NAME} PUBLIC juce::juce_audio_utils
juce::juce_dsp juce::juce_audio_processors juce::juce_audio_basics)
target_link_libraries(
  ${PROJECT_NAME} PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
                         juce::juce_recommended_warning_flags
)

target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)

set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_OPTIONS "${PROJECT_WARNINGS_CXX}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
