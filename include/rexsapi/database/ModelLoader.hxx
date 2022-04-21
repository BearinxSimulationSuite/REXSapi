
#ifndef REXSCXX_DATABASE_MODEL_LOADER_HXX
#define REXSCXX_DATABASE_MODEL_LOADER_HXX

#include <rexsapi/database/Model.hxx>
#include <rexsapi/database/ResourceLoader.hxx>

#include <functional>

namespace rexsapi::database
{
  class TModelLoader
  {
  public:
    TModelLoader() = default;
    virtual ~TModelLoader() = default;

    TModelLoader(const TModelLoader&) = delete;
    TModelLoader(TModelLoader&&) = default;
    TModelLoader& operator=(const TModelLoader&) = delete;
    TModelLoader& operator=(TModelLoader&&) = default;

    TLoaderResult load(const std::function<void(TModel)>& callback)
    {
      return doLoad(callback);
    }

  private:
    virtual TLoaderResult doLoad(const std::function<void(TModel)>& callback) = 0;
  };
}

#endif
