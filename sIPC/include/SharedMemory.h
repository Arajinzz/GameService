#include "sipc_export.h"
#include <windows.h>
#include <set>
#include <type_traits>

#include "sCore/include/Logging.h"
#include "sIPC/include/Windows.h"

namespace sIPC
{
  // DataType has to be trivially copiable
  template <typename DataType>
  class SharedMemory
  {
  public:
    template<typename... Args>
    explicit SharedMemory(Args&&... args, const std::string& name = "")
      : m_handle({nullptr, 0})
      , m_data(nullptr)
      , m_name(name)
      , m_duplicated()
    {
      static_assert(std::is_trivially_copyable_v<DataType>);
      static_assert(std::is_standard_layout_v<DataType>);
      // allocate the shared memory
      m_handle = sIPC::Windows::AllocateSharedMemory<DataType>(name);
      // construct it
      if (m_handle.Valid())
        m_data = new (m_handle.mappedAddress) DataType(std::forward<Args>(args)...);
    }

    ~SharedMemory()
    {
      // SharedMemoryHandle will release the memory automatically
    }

    bool Valid()
    {
      return m_handle.Valid();
    }

    void DuplicateToProcess(const sIPC::Windows::WinHandle& target)
    {
      auto duplicated = sIPC::Windows::DuplicateWindowsHandle(m_handle.winHandle.value, target.value);
      if (duplicated.value != 0)
        m_duplicated.push_back(std::move(duplicated));
    }

    // operator overloading for easy access
    DataType* operator->() { return m_data; }
    DataType& operator*() { return *m_data; }

  private:
    sIPC::Windows::SharedMemoryHandle m_handle;
    DataType* m_data;
    std::string m_name;
    std::vector<sIPC::Windows::WinHandle> m_duplicated;

  private:
    // delete copy and assignment
    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;
  };

} // namespace sIPC