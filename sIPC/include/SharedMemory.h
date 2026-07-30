#include "sipc_export.h"
#include <windows.h>
#include <set>
#include <type_traits>

#include "sCore/include/Logging.h"
#include "sIPC/include/Windows.h"
#include "sIPC/include/IPCObject.h"
#include "sIPC/include/Handle.h"

namespace sIPC
{
  // DataType has to be trivially copiable
  template <typename DataType, typename LogicType>
  class SharedMemory : public IPCObject
  {
  public:
    template<typename... Args>
    explicit SharedMemory(Args&&... args, const std::string& name = "")
      : IPCObject()
      , m_name(name)
      , m_memory(nullptr)
      , m_data(nullptr)
      , m_logic(nullptr)
    {
      static_assert(std::is_constructible_v<DataType, Args...>);
      static_assert(std::is_constructible_v<LogicType, DataType*>);
      static_assert(std::is_trivially_copyable_v<DataType>);
      static_assert(std::is_standard_layout_v<DataType>);
      // allocate the shared memory
      auto [handle, memory] = std::move(sIPC::Windows::AllocateSharedMemory<DataType>(name));
      m_handle = handle;
      m_memory = memory;
      // construct it
      if (m_handle.get() && m_memory.get())
      {
        m_data = new (m_memory.get()) DataType(std::forward<Args>(args)...);
        m_logic = std::make_shared<LogicType>(m_data);
      }
    }

    explicit SharedMemory(WinHandle theHandle)
      : IPCObject()
      , m_name("")
      , m_memory(nullptr)
      , m_data(nullptr)
      , m_logic(nullptr)
    {
      auto [handle, memory] = sIPC::Windows::ReadSharedMemory<DataType>(theHandle);
      m_handle = handle;
      m_memory = memory;
      // cast
      if (m_handle.get() && m_memory.get())
      {
        m_data = static_cast<DataType*>(m_memory.get());
        if (m_data)
          m_logic = std::make_shared<LogicType>(m_data);
      }
    }

    ~SharedMemory()
    {
      // Handles will release automatically
    }

    bool Valid()
    {
      return m_handle.get() && m_memory.get() && m_data && m_logic;
    }

    // operator overloading for easy access
    LogicType* operator->() 
    {
      assert(Valid());
      return m_logic.get(); 
    }

    LogicType& operator*()
    {
      assert(Valid());
      return *m_logic;
    }

  private:
    std::string m_name;
    MemHandle m_memory;
    DataType* m_data;
    std::shared_ptr<LogicType> m_logic;

  private:
    // delete copy and assignment
    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;
  };

} // namespace sIPC