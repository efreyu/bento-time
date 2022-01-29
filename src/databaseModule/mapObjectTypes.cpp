#include "mapObjectTypes.h"
#include <map>

using namespace bt::databaseModule;


const std::map<std::string, eMapObjectType> mapObjectTypes = {
    {"hero", eMapObjectType::HERO},
    {"food", eMapObjectType::FOOD}
};

eMapObjectType bt::databaseModule::getMapObjectTypeByString(const std::string& string) {
    if (mapObjectTypes.count(string)) {
        return mapObjectTypes.at(string);
    }
    return eMapObjectType::UNDEFINED;
}

