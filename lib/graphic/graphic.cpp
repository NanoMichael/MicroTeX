#include "graphic/graphic.h"
#include "utils/exceptions.h"
#include "utils/utils.h"

using namespace std;
using namespace tinytex;

namespace tinytex {

struct PlatformFactoryData {
  map<string, unique_ptr<PlatformFactory>> factories;
  string currentFactory;
};

static PlatformFactoryData TINYTEX_PLATFORM_FACTORY_DATA_INSTANCE;

} // namespace tinytex

PlatformFactoryData* PlatformFactory::_data = &TINYTEX_PLATFORM_FACTORY_DATA_INSTANCE;

void tinytex::PlatformFactory::registerFactory(const string& name, uptr<PlatformFactory> factory) {
  _data->factories[name] = std::move(factory);
}

void tinytex::PlatformFactory::activate(const std::string& name) {
  _data->currentFactory = name;
}

tinytex::PlatformFactory* tinytex::PlatformFactory::get() {
  auto it = _data->factories.find(_data->currentFactory);
  if (it == _data->factories.end()) {
    throw ex_invalid_state(
      "No factory found with name '" +
      _data->currentFactory +
      "', please register one."
    );
  }
  return it->second.get();
}
