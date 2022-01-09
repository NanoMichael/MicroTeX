#include "graphic/graphic.h"
#include "utils/exceptions.h"

using namespace std;
using namespace tex;

map<string, unique_ptr<PlatformFactory>> PlatformFactory::_factories;
string PlatformFactory::_currentFactory;

void tex::PlatformFactory::registerFactory(const string& name, unique_ptr<PlatformFactory> factory) {
  _factories[name] = std::move(factory);
}

void tex::PlatformFactory::activate(const std::string& name) {
  _currentFactory = name;
}

tex::PlatformFactory* tex::PlatformFactory::get() {
  auto it = _factories.find(_currentFactory);
  if (it == _factories.end()) {
    throw ex_invalid_state("No factory found with name '" + _currentFactory + "', please register one.");
  }
  return it->second.get();
}
