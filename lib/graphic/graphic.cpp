#include "graphic/graphic.h"

#include "utils/exceptions.h"
#include "utils/utils.h"

using namespace std;
using namespace microtex;

namespace microtex {

struct PlatformFactoryData {
  map<string, unique_ptr<PlatformFactory>> factories;
  string currentFactory;
};

static PlatformFactoryData MICROTEX_PLATFORM_FACTORY_DATA_INSTANCE;

}  // namespace microtex

PlatformFactoryData* PlatformFactory::_data = &MICROTEX_PLATFORM_FACTORY_DATA_INSTANCE;

void microtex::PlatformFactory::registerFactory(const string& name, uptr<PlatformFactory> factory) {
  _data->factories[name] = std::move(factory);
}

void microtex::PlatformFactory::activate(const std::string& name) {
  _data->currentFactory = name;
}

microtex::PlatformFactory* microtex::PlatformFactory::get() {
  auto it = _data->factories.find(_data->currentFactory);
  if (it == _data->factories.end()) {
    throw ex_invalid_state(
      "No factory found with name '" + _data->currentFactory + "', please register one."
    );
  }
  return it->second.get();
}
