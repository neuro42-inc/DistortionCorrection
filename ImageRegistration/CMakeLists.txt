cmake_minimum_required(VERSION 3.20)

PROJECT(TORTOISEV4)

SET(USEOPENMP 1)
SET(CMAKE_BUILD_TYPE Debug)
SET(ISDEBUG 1)
#SET(USECUDA 0)

if ( NOT DEFINED USECUDA)
    SET(USECUDA 0)
endif()


if ( DEFINED CMAKE_BUILD_TYPE)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        SET(ISDEBUG 0)
    else()
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            SET(ISDEBUG 1)
        else()
            SET(CMAKE_BUILD_TYPE Release)
            SET(ISDEBUG 0)
        endif()
    endif()
else()
    SET(CMAKE_BUILD_TYPE Release)
    SET(ISDEBUG 0)
endif()


message( "${CMAKE_CXX_COMPILER}")
message( "${CMAKE_C_COMPILER}")
message( "${CMAKE_BUILD_TYPE}")

#SET(CMAKE_C_COMPILER /usr/bin/gcc-9)
#SET(CMAKE_CXX_COMPILER /usr/bin/g++-9)

message(${CMAKE_BINARY_DIR})
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/lib/ )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/lib/ )



IF(USEOPENMP)
    SET(CMAKE_CXX_FLAGS "-fopenmp -std=c++14 -Wno-deprecated-declarations -Wno-unused-result -Wno-narrowing")
    SET(CMAKE_C_FLAGS "-fopenmp -Wno-deprecated-declarations -Wno-unused-result -Wno-narrowing")
    add_definitions(-DUSE_OPENMP)
ELSE(USEOPENMP)
    SET(CMAKE_CXX_FLAGS "-fPIC -std=c++14 -Wno-deprecated-declarations -Wno-unused-result -Wno-narrowing")
    SET(CMAKE_C_FLAGS "-fPIC -Wno-deprecated-declarations -Wno-unused-result -Wno-narrowing")
ENDIF(USEOPENMP)



set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_NO_BOOST_CMAKE ON)
set(BOOST_ROOT /usr/local/boost176)
set(Boost_LIBRARY_DIRS /usr/local/boost176/lib)
set(Boost_INCLUDE_DIRS /usr/local/boost176/include)
find_package(Boost COMPONENTS iostreams filesystem system REQUIRED)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
  MESSAGE("Boost Found")
else(Boost_FOUND)
   MESSAGE("BOOST NOT FOUND!")
endif()



IF(USECUDA)
    include(CheckLanguage)
    set(CMAKE_CUDA_COMPILER "/usr/local/cuda/bin/nvcc")

    set(CUDA_TOOLKIT_ROOT_DIR "/usr/local/cuda")
    set(CMAKE_CUDA_COMPILER "/usr/local/cuda/bin/nvcc")
    set(CMAKE_CUDA_COMPILER_TOOLKIT_ROOT "/usr/local/cuda")
    set(CMAKE_CUDA_COMPILER_LIBRARY_ROOT "/usr/local/cuda")
    set(CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES "/usr/local/cuda/targets/x86_64-linux/include/")
    set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)

    find_package(CUDAToolkit)

    check_language(CUDA)
    message(${CMAKE_CUDA_COMPILER})
    if(CMAKE_CUDA_COMPILER)
      enable_language(CUDA)
      add_definitions(-DUSECUDA)
      message("CUDA Enabled..")
    else()
      message(STATUS "No CUDA SUPPORT..")
    endif()
ENDIF(USECUDA)


# Find ITK.
IF(ISDEBUG)
     if ( NOT DEFINED ITK_DIR)
        SET(ITK_DIR /home/juan_liu/Documents/Projects/tools/InsightToolkit-5.3rc04_build)
    endif()
    FIND_PACKAGE(ITK)
    IF(ITK_FOUND)
        INCLUDE(${ITK_USE_FILE})
        MESSAGE("ITK ${ITK_VERSION_MAJOR}${ITK_VERSION_MINOR}${ITK_VERSION_PATCH} Debug version used!")
    ELSE(ITK_FOUND)
        MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
    ENDIF(ITK_FOUND)
ELSE(ISDEBUG)
    if ( NOT DEFINED ITK_DIR)
        SET(ITK_DIR /home/juan_liu/Documents/Projects/tools/InsightToolkit-5.3rc04_build)
    endif()
    FIND_PACKAGE(ITK)
    IF(ITK_FOUND)
       INCLUDE(${ITK_USE_FILE})
       MESSAGE("ITK ${ITK_VERSION_MAJOR}${ITK_VERSION_MINOR}${ITK_VERSION_PATCH} Release version used!")
    ELSE(ITK_FOUND)
        MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
    ENDIF(ITK_FOUND)
ENDIF(ISDEBUG)


find_package (Eigen3 3.3 REQUIRED NO_MODULE)
if(Eigen3_FOUND)
    message("Eigen found.")
    message(${EIGEN3_INCLUDE_DIR})
    include_directories(${EIGEN3_INCLUDE_DIR})
endif()


set(SOURCES defines.cxx  TORTOISE_global.cxx antsCommandLineParser.cxx antsCommandLineOption.cxx  ./utilities/TORTOISE_Utilities.cxx ./utilities/extract_3Dvolume_from_4D.cxx  ./utilities/read_3Dvolume_from_4D.cxx ./utilities/read_bmatrix_file.cxx ./utilities/math_utilities.cxx ./external_src/cmpfit-1.3a/mpfit.c)


IF(USECUDA)
     include_directories(./cuda_src )

     add_executable(DRBUDDI_cuda DRBUDDI_main.cxx DRBUDDI_parserBase.cxx ./tools/EstimateTensor/DTIModel.cxx DRBUDDI.cxx DRBUDDIBase.cxx   ./cuda_src/cuda_image.cxx  ./cuda_src/warp_image.cu ./cuda_src/warp_image.cxx  run_drbuddi_stage.cxx run_drbuddi_stage_TVVF.cxx ./cuda_src/resample_image.cu ./cuda_src/resample_image.cxx ./cuda_src/gaussian_smooth_image.cu ./cuda_src/gaussian_smooth_image.cxx  ./cuda_src/cuda_image_utilities.cu ./cuda_src/cuda_image_utilities.cxx ./cuda_src/compute_metric.cu ./cuda_src/compute_metric.cxx DRBUDDI_Diffeo.cxx   ./tools/ResampleDWIs/resample_dwis.cxx rigid_register_images.cxx ./tools/EstimateTensor/estimate_tensor_wlls.cxx create_mask.cxx    ${SOURCES} )
     target_include_directories(DRBUDDI_cuda  PRIVATE    ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
     target_link_libraries(DRBUDDI_cuda  ${ITK_LIBRARIES} ${Boost_LIBRARIES}  -static-libgcc -static-libstdc++)
     target_compile_definitions(DRBUDDI_cuda PUBLIC -DDRBUDDIALONE)


ELSE(USECUDA)
    #if(NOT QSIPREP)
    #SET(CMAKE_EXE_LINKER_FLAGS "-static ")
    #endif()

     add_executable(DRBUDDI DRBUDDI_main.cxx DRBUDDI_parserBase.cxx ./tools/EstimateTensor/DTIModel.cxx   DRBUDDI.cxx DRBUDDIBase.cxx   run_drbuddi_stage.cxx DRBUDDI_Diffeo.cxx   ./tools/ResampleDWIs/resample_dwis.cxx rigid_register_images.cxx ./tools/EstimateTensor/estimate_tensor_wlls.cxx create_mask.cxx  drbuddi_image_utilities.cxx  ${SOURCES} )
     target_link_libraries(DRBUDDI  ${ITK_LIBRARIES}  ${Boost_LIBRARIES} fftw3)
     target_compile_definitions(DRBUDDI PUBLIC -DDRBUDDIALONE)


ENDIF(USECUDA)


