#ifndef NEMO_CONFIG_H
#define NEMO_CONFIG_H

#define HAVE_BUILTIN_CLZLL
#define HAVE_BUILTIN_CLZ
#define HAVE_BUILTIN_CTZLL
#define HAVE_LOG2
#define HAVE_POSIX_MEMALIGN
/* #undef HAVE_CUPRINTF */
/* #undef NEMO_MPI_ENABLED */
/* #undef NEMO_MPI_DEBUG_TRACE */
/* #undef NEMO_MPI_DEBUG_TIMING */
/* #undef NEMO_MPI_COMMUNICATION_COUNTERS */
#define NEMO_CUDA_ENABLED
#define NEMO_CUDA_DYNAMIC_LOADING
/* #undef NEMO_CUDA_DEBUG_TRACE */
/* #undef NEMO_CUDA_DEVICE_ASSERTIONS */
/* #undef NEMO_CPU_DEBUG_TRACE */
#define NEMO_CPU_OPENMP_ENABLED
/* #undef NEMO_BRIAN_ENABLED */
#define NEMO_WEIGHT_FIXED_POINT_SATURATION

#define NEMO_VERSION "0.7.2"
#if defined _WIN32 || defined __CYGWIN__
/* The system plugin directory is relative to the dll location */
#define NEMO_SYSTEM_PLUGIN_DIR "lib/nemo/plugins"
#else
#define NEMO_SYSTEM_PLUGIN_DIR "/usr/local/lib/nemo/plugins"
#endif
#define NEMO_USER_PLUGIN_DIR ".nemo/plugins"

const unsigned NEMO_MAX_NEURON_ARGS = 16;


#if defined _WIN32 || defined __CYGWIN__
  #ifdef NEMO_EXPORTS
    #ifdef __GNUC__
      #define NEMO_DLL_PUBLIC __attribute__((dllexport))
    #else
      #define NEMO_DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define NEMO_DLL_PUBLIC __attribute__((dllimport))
    #else
      #define NEMO_DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
#else
  #define NEMO_DLL_PUBLIC
  #define NEMO_DLL_LOCAL
#endif

#if defined _WIN32 || defined __CYGWIN__
  #ifdef NEMO_BASE_EXPORTS
    #ifdef __GNUC__
      #define NEMO_BASE_DLL_PUBLIC __attribute__((dllexport))
    #else
      #define NEMO_BASE_DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define NEMO_BASE_DLL_PUBLIC __attribute__((dllimport))
    #else
      #define NEMO_BASE_DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
#else
  #define NEMO_BASE_DLL_PUBLIC
  #define NEMO_BASE_DLL_LOCAL
#endif

#if defined _WIN32 || defined __CYGWIN__
  #ifdef NEMO_CUDA_EXPORTS
    #ifdef __GNUC__
      #define NEMO_CUDA_DLL_PUBLIC __attribute__((dllexport))
    #else
      #define NEMO_CUDA_DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define NEMO_CUDA_DLL_PUBLIC __attribute__((dllimport))
    #else
      #define NEMO_CUDA_DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
#else
  #define NEMO_CUDA_DLL_PUBLIC
  #define NEMO_CUDA_DLL_LOCAL
#endif

#if defined _WIN32 || defined __CYGWIN__
  #ifdef NEMO_CPU_EXPORTS
    #ifdef __GNUC__
      #define NEMO_CPU_DLL_PUBLIC __attribute__((dllexport))
    #else
      #define NEMO_CPU_DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define NEMO_CPU_DLL_PUBLIC __attribute__((dllimport))
    #else
      #define NEMO_CPU_DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
#else
  #define NEMO_CPU_DLL_PUBLIC
  #define NEMO_CPU_DLL_LOCAL
#endif

#if defined _WIN32 || defined __CYGWIN__
  #ifdef NEMO_PLUGIN_EXPORTS
    #ifdef __GNUC__
      #define NEMO_PLUGIN_DLL_PUBLIC __attribute__((dllexport))
    #else
      #define NEMO_PLUGIN_DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define NEMO_PLUGIN_DLL_PUBLIC __attribute__((dllimport))
    #else
      #define NEMO_PLUGIN_DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
#else
  #define NEMO_PLUGIN_DLL_PUBLIC
  #define NEMO_PLUGIN_DLL_LOCAL
#endif

#if defined(_WIN32) && (defined(NEMO_BASE_EXPORTS) || defined(NEMO_CUDA_EXPORTS) || defined(NEMO_CPU_EXPORTS) || defined(NEMO_PLUGIN_EXPORTS) || defined(NEMO_EXPORTS))
/* Suppress useless warning about DLL exporting of template classes */
#pragma warning(disable:4251)

/* Suppress warnings about the great evils of the standard libraries */
#pragma warning(disable:4996)

/* Suppress warnings about non dll-interface class used as base for dll-interface class */
#pragma warning(disable:4725)
#endif


#endif
