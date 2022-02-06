#ifndef BENTO_TIME_PROGRESSPROFILEBLOCK_H
#define BENTO_TIME_PROGRESSPROFILEBLOCK_H

#include "generic/profileModule/profileBlockInterface.h"
#include "generic/utilityModule/jsonHelper.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace bt::profileModule {

    struct sProgressProfile {
        int moves = 0;

        bool load(const rapidjson::GenericValue<rapidjson::UTF8<char>>::ConstObject&);
        bool save(rapidjson::Value&, rapidjson::Document::AllocatorType&);
        void addProgress(int progress);
    };

    class progressProfileBlock : public generic::profileModule::profileBlockInterface {
    public:
        ~progressProfileBlock() override;

        bool load(const rapidjson::GenericValue<rapidjson::UTF8<char>>::ConstObject&) override;
        bool save(rapidjson::Value&, rapidjson::Document::AllocatorType&) override;

        std::map<int, sProgressProfile*>& getAllProgress() {
            return progressByLevels;
        }

        sProgressProfile* getProgressForLevel(int);

    private:
        std::map<int, sProgressProfile*> progressByLevels; // level and progress data
    };
}// namespace bt::profileModule


#endif// BENTO_TIME_PROGRESSPROFILEBLOCK_H
