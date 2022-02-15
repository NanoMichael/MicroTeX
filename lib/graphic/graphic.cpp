#include "graphic/graphic.h"
#include "utils/exceptions.h"

using namespace std;
using namespace tinytex;

map<string, unique_ptr<PlatformFactory>> PlatformFactory::_factories;
string PlatformFactory::_currentFactory;

void tinytex::PlatformFactory::registerFactory(const string& name, unique_ptr<PlatformFactory> factory) {
  _factories[name] = std::move(factory);
}

void tinytex::PlatformFactory::activate(const std::string& name) {
  _currentFactory = name;
}

tinytex::PlatformFactory* tinytex::PlatformFactory::get() {
  auto it = _factories.find(_currentFactory);
  if (it == _factories.end()) {
    throw ex_invalid_state("No factory found with name '" + _currentFactory + "', please register one.");
  }
  return it->second.get();
}
