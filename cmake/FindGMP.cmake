# FindGMP.cmake

# Locate GMP library

# This module defines
#  GMP_INCLUDE_DIR, where to find gmp.h, etc.
#  GMP_LIBRARIES, the libraries to link against to use GMP.
#  GMP_FOUND, If false, do not try to use GMP.

find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARIES NAMES gmp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_INCLUDE_DIR GMP_LIBRARIES)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARIES)