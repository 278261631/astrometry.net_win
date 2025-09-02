cmake_minimum_required(VERSION 3.16)
project(astrometry_net C)

# 设置编译器路径
set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")

# 设置CMAKE路径
set(CMAKE_COMMAND "C:/CMake/bin/cmake.exe")

# 设置Debug模式，禁用优化
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/../include/astrometry
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../util
    ${CMAKE_SOURCE_DIR}/../solver
    ${CMAKE_SOURCE_DIR}/../catalogs
    ${CMAKE_SOURCE_DIR}/../libkd
    ${CMAKE_SOURCE_DIR}/../qfits-an
    ${CMAKE_SOURCE_DIR}/../gsl-an
    ${CMAKE_SOURCE_DIR}  # For Windows mman.h
)

# 查找必要的库
find_package(PkgConfig REQUIRED)

# 查找cfitsio
pkg_check_modules(CFITSIO REQUIRED cfitsio)
if(CFITSIO_FOUND)
    include_directories(${CFITSIO_INCLUDE_DIRS})
    link_directories(${CFITSIO_LIBRARY_DIRS})
endif()

# 查找GSL (如果系统有的话)
pkg_check_modules(GSL gsl)
if(NOT GSL_FOUND)
    message(STATUS "System GSL not found, will use bundled gsl-an")
    set(USE_BUNDLED_GSL TRUE)
endif()

# 定义源文件组
# qfits-an 库
set(QFITS_SOURCES
    ../qfits-an/anqfits.c
    ../qfits-an/qfits_tools.c
    ../qfits-an/qfits_table.c
    ../qfits-an/qfits_float.c
    ../qfits-an/qfits_error.c
    ../qfits-an/qfits_time.c
    ../qfits-an/qfits_card.c
    ../qfits-an/qfits_header.c
    ../qfits-an/qfits_rw.c
    ../qfits-an/qfits_memory.c
    ../qfits-an/qfits_convert.c
    ../qfits-an/qfits_byteswap.c
    ../qfits-an/qfits_image.c
    ../qfits-an/qfits_md5.c
    ../qfits-an/md5.c
)

# Windows mman implementation
if(WIN32)
    set(QFITS_SOURCES ${QFITS_SOURCES} mman.c)
endif()

# util/anbase 库
set(ANBASE_SOURCES
    ../util/ioutils.c
    ../util/mathutil.c
    ../util/fitsioutils.c
    ../util/fitsbin.c
    ../util/an-endian.c
    ../util/fitsfile.c
    ../util/log.c
    ../util/errors.c
    ../util/tic.c
    ../util/bl.c
    ../util/bl-sort.c
)

# util/anutils 库
set(ANUTILS_SOURCES
    ../util/starutil.c
    ../util/sip.c
    ../util/sip_qfits.c
    ../util/sip-utils.c
    ../util/fit-wcs.c
    ../util/gslutils.c
    ../util/xylist.c
    ../util/datalog.c
    ../util/fileutils.c
    ../util/permutedsort.c
    ../util/resample.c
    ../util/ctmf.c
    ../util/dselip.c
    ../util/dsmooth.c
    ../util/dallpeaks.c
    ../util/dcen3x3.c
    ../util/dfind.c
    ../util/dobjects.c
    ../util/dmedsmooth.c
    ../util/dpeaks.c
    ../util/dsigma.c
    ../util/simplexy.c
    ../util/image2xy.c
    ../util/healpix.c
    ../util/an-opts.c
    ../util/wcs-rd2xy.c
    ../util/scamp.c
    ../util/tabsort.c
    ../util/anwcs.c
)

# util/anfiles 库
set(ANFILES_SOURCES
    ../util/index.c
    ../util/indexset.c
    ../util/multiindex.c
    ../util/codekd.c
    ../util/quadfile.c
    ../util/matchfile.c
    ../util/matchobj.c
    ../util/rdlist.c
    ../util/starkd.c
    ../util/starxy.c
    ../util/scamp-catalog.c
    ../util/fitstable.c
)

# libkd 库
set(LIBKD_SOURCES
    ../libkd/kdtree.c
    ../libkd/kdtree_dim.c
    ../libkd/kdtree_fits_io.c
    ../libkd/kdint_ddd.c
    ../libkd/kdint_dds.c
    ../libkd/kdint_ddu.c
    ../libkd/kdint_dss.c
    ../libkd/kdint_duu.c
    ../libkd/kdint_fff.c
    ../libkd/kdint_lll.c
)

# catalogs 库
set(CATALOGS_SOURCES
    ../catalogs/brightstars.c
    ../catalogs/constellation-boundaries.c
    ../catalogs/constellations.c
    ../catalogs/hd.c
    ../catalogs/nomad.c
    ../catalogs/tycho2.c
    ../catalogs/ucac3.c
    ../catalogs/ucac4.c
    ../catalogs/usnob.c
    ../catalogs/2mass.c
    # ../catalogs/openngc.c  # Requires generated files, skip for Windows build
    ../catalogs/stellarium-constellations.c
)

# GSL库 (如果使用bundled版本)
if(USE_BUNDLED_GSL)
    set(GSL_SOURCES
        ../gsl-an/vector/init.c
        ../gsl-an/vector/vector.c
        ../gsl-an/matrix/init.c
        ../gsl-an/matrix/matrix.c
        ../gsl-an/matrix/getset.c
        ../gsl-an/matrix/copy.c
        ../gsl-an/matrix/swap.c
        ../gsl-an/matrix/prop.c
        ../gsl-an/matrix/file.c
        ../gsl-an/matrix/rowcol.c
        ../gsl-an/matrix/submatrix.c
        ../gsl-an/matrix/view.c
        ../gsl-an/linalg/linalg.c
        ../gsl-an/linalg/lu.c
        ../gsl-an/linalg/luc.c
        ../gsl-an/linalg/hh.c
        ../gsl-an/linalg/qr.c
        ../gsl-an/linalg/qrpt.c
        ../gsl-an/linalg/lq.c
        ../gsl-an/linalg/ptlq.c
        ../gsl-an/linalg/svd.c
        ../gsl-an/linalg/householder.c
        ../gsl-an/linalg/householdercomplex.c
        ../gsl-an/linalg/hessenberg.c
        ../gsl-an/linalg/hesstri.c
        ../gsl-an/linalg/cholesky.c
        ../gsl-an/linalg/choleskyc.c
        ../gsl-an/linalg/symmtd.c
        ../gsl-an/linalg/hermtd.c
        ../gsl-an/linalg/bidiag.c
        ../gsl-an/linalg/balance.c
        ../gsl-an/linalg/balancemat.c
        ../gsl-an/permutation/init.c
        ../gsl-an/permutation/permutation.c
        ../gsl-an/permutation/permute.c
        ../gsl-an/permutation/canonical.c
        ../gsl-an/blas/blas.c
        ../gsl-an/cblas/cblas.c
        ../gsl-an/block/init.c
        ../gsl-an/block/block.c
        ../gsl-an/block/file.c
        ../gsl-an/err/error.c
        ../gsl-an/err/stream.c
        ../gsl-an/err/message.c
        ../gsl-an/err/strerror.c
        ../gsl-an/sys/coerce.c
        ../gsl-an/sys/fdiv.c
        ../gsl-an/sys/fcmp.c
        ../gsl-an/sys/ldfrexp.c
        ../gsl-an/sys/minmax.c
        ../gsl-an/sys/prec.c
        ../gsl-an/sys/hypot.c
        ../gsl-an/sys/log1p.c
        ../gsl-an/sys/expm1.c
        ../gsl-an/sys/pow_int.c
        ../gsl-an/sys/infnan.c
    )
endif()

# solver 核心源文件
set(SOLVER_ENGINE_SOURCES
    ../solver/engine.c
    ../solver/solverutils.c
    ../solver/onefield.c
    ../solver/solver.c
    ../solver/quad-utils.c
    ../solver/solvedfile.c
    ../solver/tweak2.c
    ../solver/verify.c
    ../solver/tweak.c
    ../solver/new-wcs.c
    ../solver/fits-guess-scale.c
    ../solver/cut-table.c
    ../solver/resort-xylist.c
    ../solver/build-index.c
    ../solver/uniformize-catalog.c
    ../solver/startree.c
    ../solver/hpquads.c
    ../solver/quad-builder.c
    ../solver/codefile.c
    ../solver/codetree.c
    ../solver/unpermute-stars.c
    ../solver/unpermute-quads.c
    ../solver/merge-index.c
    ../solver/augment-xylist.c
    ../solver/image2xy-files.c
)

# 创建静态库
add_library(qfits STATIC ${QFITS_SOURCES})
add_library(anbase STATIC ${ANBASE_SOURCES})
add_library(anutils STATIC ${ANUTILS_SOURCES})
add_library(anfiles STATIC ${ANFILES_SOURCES})
add_library(libkd STATIC ${LIBKD_SOURCES})
add_library(catalogs STATIC ${CATALOGS_SOURCES})

if(USE_BUNDLED_GSL)
    add_library(gsl-an STATIC ${GSL_SOURCES})
endif()

add_library(astrometry STATIC ${SOLVER_ENGINE_SOURCES})

# 设置库的依赖关系
target_link_libraries(anbase qfits)
target_link_libraries(anutils anbase qfits)
target_link_libraries(anfiles anutils anbase qfits libkd)
target_link_libraries(libkd anbase)
target_link_libraries(catalogs anfiles anutils anbase qfits libkd)

if(USE_BUNDLED_GSL)
    target_link_libraries(anutils gsl-an)
    target_link_libraries(astrometry catalogs anfiles anutils anbase qfits libkd gsl-an)
else()
    target_link_libraries(anutils ${GSL_LIBRARIES})
    target_link_libraries(astrometry catalogs anfiles anutils anbase qfits libkd ${GSL_LIBRARIES})
endif()

# 创建solve-field可执行文件
add_executable(solve-field ../solver/solve-field.c)
target_link_libraries(solve-field
    astrometry
    catalogs
    anutils
    anfiles
    anbase
    qfits
    libkd
    ${CFITSIO_LIBRARIES}
    m
)

if(USE_BUNDLED_GSL)
    target_link_libraries(solve-field gsl-an)
else()
    target_link_libraries(solve-field ${GSL_LIBRARIES})
endif()

# 添加编译定义
target_compile_definitions(solve-field PRIVATE
    -DHAVE_NETPBM=0
    -DNDEBUG=0
)

# 设置Windows特定的链接选项
if(WIN32)
    target_link_libraries(solve-field ws2_32)
    # 尝试链接regex库
    find_library(REGEX_LIBRARY NAMES regex)
    if(REGEX_LIBRARY)
        target_link_libraries(solve-field ${REGEX_LIBRARY})
    endif()
endif()

# 复制DLL文件的自定义目标
add_custom_target(copy_dlls ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Copying required DLLs..."
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "C:/msys64/mingw64/bin/libgcc_s_seh-1.dll" 
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "C:/msys64/mingw64/bin/libwinpthread-1.dll" 
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libcfitsio-10.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libgsl-28.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libsystre-0.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libcurl-4.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libintl-8.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libtre-5.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libbrotlidec.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libbrotlicommon.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libiconv-2.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libidn2-0.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    # 添加更多可能缺失的DLL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libssl-3-x64.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libcrypto-3-x64.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libnghttp2-14.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libpsl-5.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libunistring-5.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libzstd.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libssh2-1.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libnghttp3-9.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libngtcp2-16.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libngtcp2_crypto_ossl-0.dll"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Copying DLL dependencies"
)

# 确保在构建solve-field后复制DLL
add_dependencies(copy_dlls solve-field)
