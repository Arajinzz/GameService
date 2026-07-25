#pragma once

#include "sipc_export.h"
#include "sCore/include/Logging.h"

#include <windows.h>
#include <string.h>
#include <chrono>
#include <memory>

namespace sIPC
{
  enum class MessageType : unsigned
  {

  };

  struct MessageMetaData
  {
    unsigned id;
    MessageType type;
  };

} // namespace sIPC
