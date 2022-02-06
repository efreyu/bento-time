#include "progressProfileBlock.h"
#include <algorithm>

using namespace bt::profileModule;
using namespace rapidjson;


progressProfileBlock::~progressProfileBlock() {
    for (auto& item : progressByLevels) {
        delete item.second;
        item.second = nullptr;
    }
    progressByLevels.clear();
}

bool progressProfileBlock::load(const GenericValue<UTF8<char>>::ConstObject& data) {
    for (auto it = data.MemberBegin(); it != data.MemberEnd(); ++it) {
        int level = std::atoi(it->name.GetString());
        if (level > 0 && it->value.IsObject()) {
            auto obj = it->value.GetObject();
            auto item = new sProgressProfile();
            if (item->load(obj)) {
                progressByLevels.insert({ level, item });
            } else {
                delete item;
            }
        }
    }
    return true;
}

bool progressProfileBlock::save(Value& data, Document::AllocatorType& allocator) {
    for (const auto& [id, info]  : progressByLevels) {
        Value object;
        object.SetObject();
        if (info->save(object, allocator)) {
            rapidjson::Value key(std::to_string(id).c_str(), allocator);
            data.AddMember(key, object, allocator);
        }
    }
    return true;
}

sProgressProfile* progressProfileBlock::getProgressForLevel(int id) {
    if (progressByLevels.count(id)) {
        return progressByLevels[id];
    } else {
        auto item = new sProgressProfile();
        progressByLevels.insert({ id, item });
        return progressByLevels[id];
    }
}

bool sProgressProfile::load(const GenericValue<UTF8<char>>::ConstObject& data) {
    auto _moves = data.FindMember("moves");
    if (_moves != data.MemberEnd() && _moves->value.IsInt()) {
        moves = _moves->value.GetInt();
    } else {
        return false;
    }
    return true;
}

bool sProgressProfile::save(Value& data, Document::AllocatorType& allocator) {
    data.AddMember("moves", moves, allocator);
    return true;
}

void sProgressProfile::addProgress(int progress) {
    if (moves == 0 || progress < moves) {
        moves = progress;
    }
}
