# CMake script to copy NetPBM and GAWK tools if they exist
# This script is called during the build process

set(GNUWIN32_BIN_DIR "C:/GnuWin32/bin")
set(DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

message(STATUS "Copying NetPBM and GAWK tools if available...")
message(STATUS "Source directory: ${GNUWIN32_BIN_DIR}")
message(STATUS "Destination directory: ${DEST_DIR}")

# List of NetPBM tools to copy
set(NETPBM_TOOLS
    "bmptopnm.exe"
    "jpegtopnm.exe" 
    "pnmtopgm.exe"
    "ppmtopgm.exe"
    "pnmfile.exe"
    "pngtopnm.exe"
    "tifftopnm.exe"
    "giftopnm.exe"
    "fitstopnm.exe"
    "pnmtofits.exe"
    "pamtopnm.exe"
    "pnmtopam.exe"
    "pnmscale.exe"
    "pnmflip.exe"
    "pnmrotate.exe"
    "pnmcrop.exe"
)

# List of GAWK tools to copy
set(GAWK_TOOLS
    "gawk.exe"
    "awk.exe"
)

# List of DLL files to copy
set(NETPBM_DLLS
    "jpeg62.dll"
    "libpng13.dll"
    "libtiff3.dll"
    "zlib1.dll"
    "libungif4.dll"
    "msvcr71.dll"
)

# Function to copy file if it exists
function(copy_if_exists source_file dest_dir tool_name)
    if(EXISTS "${source_file}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${source_file}" "${dest_dir}"
            RESULT_VARIABLE copy_result
        )
        if(copy_result EQUAL 0)
            message(STATUS "  ✓ Copied ${tool_name}")
        else()
            message(STATUS "  ✗ Failed to copy ${tool_name}")
        endif()
    else()
        message(STATUS "  - ${tool_name} not found, skipping")
    endif()
endfunction()

# Copy NetPBM tools
message(STATUS "Checking NetPBM tools:")
foreach(tool ${NETPBM_TOOLS})
    set(source_file "${GNUWIN32_BIN_DIR}/${tool}")
    copy_if_exists("${source_file}" "${DEST_DIR}" "${tool}")
endforeach()

# Copy GAWK tools
message(STATUS "Checking GAWK tools:")
foreach(tool ${GAWK_TOOLS})
    set(source_file "${GNUWIN32_BIN_DIR}/${tool}")
    copy_if_exists("${source_file}" "${DEST_DIR}" "${tool}")
endforeach()

# Copy DLL files
message(STATUS "Checking NetPBM DLL files:")
foreach(dll ${NETPBM_DLLS})
    set(source_file "${GNUWIN32_BIN_DIR}/${dll}")
    copy_if_exists("${source_file}" "${DEST_DIR}" "${dll}")
endforeach()

# Also check for tools in system PATH
message(STATUS "Checking system PATH for additional tools...")

# Try to find jpegtopnm in PATH
find_program(JPEGTOPNM_PATH jpegtopnm)
if(JPEGTOPNM_PATH AND NOT EXISTS "${DEST_DIR}/jpegtopnm.exe")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${JPEGTOPNM_PATH}" "${DEST_DIR}"
        RESULT_VARIABLE copy_result
    )
    if(copy_result EQUAL 0)
        message(STATUS "  ✓ Copied jpegtopnm from PATH: ${JPEGTOPNM_PATH}")
    endif()
endif()

# Try to find gawk in PATH
find_program(GAWK_PATH gawk)
if(GAWK_PATH AND NOT EXISTS "${DEST_DIR}/gawk.exe")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GAWK_PATH}" "${DEST_DIR}"
        RESULT_VARIABLE copy_result
    )
    if(copy_result EQUAL 0)
        message(STATUS "  ✓ Copied gawk from PATH: ${GAWK_PATH}")
    endif()
endif()

message(STATUS "NetPBM and GAWK tools copy process completed.")
