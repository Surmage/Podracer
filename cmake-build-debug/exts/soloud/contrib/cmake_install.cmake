# Install script for directory: /home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/aleslu-0/CLionProjects/rpi_engine/cmake-build-debug/exts/soloud/contrib/libsoloud.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/soloud" TYPE FILE FILES
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_audiosource.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_bassboostfilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_biquadresonantfilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_bus.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_c.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_dcremovalfilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_echofilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_error.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_fader.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_fft.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_fftfilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_file.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_file_hack_off.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_file_hack_on.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_filter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_flangerfilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_internal.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_lofifilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_monotone.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_openmpt.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_queue.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_robotizefilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_sfxr.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_speech.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_tedsid.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_thread.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_vic.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_vizsn.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_wav.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_waveshaperfilter.h"
    "/home/aleslu-0/CLionProjects/rpi_engine/exts/soloud/contrib/../include/soloud_wavstream.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/SoLoud/cmake/soloud-config.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/SoLoud/cmake/soloud-config.cmake"
         "/home/aleslu-0/CLionProjects/rpi_engine/cmake-build-debug/exts/soloud/contrib/CMakeFiles/Export/9b9456137e963844a71c12435b0b376d/soloud-config.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/SoLoud/cmake/soloud-config-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/SoLoud/cmake/soloud-config.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SoLoud/cmake" TYPE FILE FILES "/home/aleslu-0/CLionProjects/rpi_engine/cmake-build-debug/exts/soloud/contrib/CMakeFiles/Export/9b9456137e963844a71c12435b0b376d/soloud-config.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SoLoud/cmake" TYPE FILE FILES "/home/aleslu-0/CLionProjects/rpi_engine/cmake-build-debug/exts/soloud/contrib/CMakeFiles/Export/9b9456137e963844a71c12435b0b376d/soloud-config-debug.cmake")
  endif()
endif()

