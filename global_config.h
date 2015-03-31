/*
  Global config
  Only do #define and typedef here
  NO VARIABLE IS ALLOWED because that will cause
  multiple definitons
  */

#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#if defined(WIN32) || defined(_WIN64)
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#define DEBUG
#define LOG_ENABLED
//#define USE_TRIGGER_ARRAY
#endif

#endif
