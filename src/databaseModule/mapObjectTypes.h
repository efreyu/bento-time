#ifndef BENTO_TIME_MAPOBJECTTYPES_H
#define BENTO_TIME_MAPOBJECTTYPES_H

#include <string>

namespace bt::databaseModule {

    enum class eMapObjectType {
        UNDEFINED = 0,
        HERO,
        FOOD
    };

    eMapObjectType getMapObjectTypeByString(const std::string& string);
}

#endif// BENTO_TIME_MAPOBJECTTYPES_H
