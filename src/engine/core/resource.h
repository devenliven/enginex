#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <memory>
#include <string>

class IResource
{
  public:
    virtual ~IResource()                         = default;
    virtual bool   load(const std::string& path) = 0;
    virtual void   unload()                      = 0;
    virtual bool   isLoaded() const              = 0;
    virtual size_t getMemoryUsage() const        = 0;

    const std::string& getPath() const { return m_path; }

  protected:
    std::string m_path;
};

using ResourcePtr = std::shared_ptr<IResource>;

#endif // RESOURCE_H_