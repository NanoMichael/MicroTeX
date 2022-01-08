#include "graphic/graphic.h"
#include "utils/exceptions.h"

std::map<std::string, tex::PlatformFactory*> tex::PlatformFactory::_factories;
std::string tex::PlatformFactory::_currentFactory;

void tex::PlatformFactory::registerFactory(const std::string& name, PlatformFactory* factory) {
  auto it = _factories.find(name);
  if (it != _factories.end()) {
    delete it->second;
  }
  _factories[name] = factory;
}

void tex::PlatformFactory::activate(const std::string& name) {
  _currentFactory = name;
}

tex::PlatformFactory* tex::PlatformFactory::get() {
  auto it = _factories.find(_currentFactory);
  if (it == _factories.end()) {
    throw ex_invalid_state("No factory found with name '" + _currentFactory + "', please register one.");
  }
  return it->second;
}
