# FindMPFR.cmake

# Locate MPFR library

# This module defines
#  MPFR_INCLUDE_DIR, where to find mpfr.h, etc.
#  MPFR_LIBRARIES, the libraries to link against to use MPFR.
#  MPFR_FOUND, If false, do not try to use MPFR.

find_path(MPFR_INCLUDE_DIR NAMES mpfr.h)
find_library(MPFR_LIBRARIES NAMES mpfr)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPFR DEFAULT_MSG MPFR_INCLUDE_DIR MPFR_LIBRARIES)

mark_as_advanced(MPFR_INCLUDE_DIR MPFR_LIBRARIES)