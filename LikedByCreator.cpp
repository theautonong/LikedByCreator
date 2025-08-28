#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {
    // Dropdown for choosing icon style
    Mod::get()->addCustomSetting(
        "creatorIcon",
        "Creator Face,Star Icon,Like Button,Gold Star"
    );

    // Toggle for showing "(Liked by Creator)" text
    Mod::get()->addCustomSetting("showCreatorText", true);

    // Offset between username text and icon/text
    Mod::get()->addCustomSetting("textIconGap", 6.0f, 0.0f, 20.0f);

    // Maximum scale for creator icon
    Mod::get()->addCustomSetting("maxIconScale", 0.5f, 0.1f, 1.0f);

    // Text color for "(Liked by Creator)" as RGB sliders
    Mod::get()->addCustomSetting("creatorTextColorR", 255.0f, 0.0f, 255.0f);
    Mod::get()->addCustomSetting("creatorTextColorG", 215.0f, 0.0f, 255.0f);
    Mod::get()->addCustomSetting("creatorTextColorB", 0.0f, 0.0f, 255.0f);

    // Add preview layer
    Mod::get()->addCustomSetting("previewLayer", "", [](CCNode* parent){
        auto bg = CCLayerColor::create({50, 50, 50, 255}, 300, 60);
        bg->setPosition({10, 10});
        parent->addChild(bg);

        auto username = CCLabelBMFont::create("Username", "chatFont.fnt");
        username->setScale(0.5f);
        username->setAnchorPoint({0.f, 0.5f});
        username->setPosition({10, 40});
        bg->addChild(username);

        bool showText = Mod::get()->getSettingValue<bool>("showCreatorText");
        float gap = Mod::get()->getSettingValue<float>("textIconGap");
        float xPosition = username->getPositionX() + username->getContentSize().width + gap;

        if (showText) {
            auto extraLabel = CCLabelBMFont::create("(Liked by Creator)", "chatFont.fnt");
            extraLabel->setScale(0.5f);
            extraLabel->setAnchorPoint({0.f, 0.5f});

            // Get color from settings
            float r = Mod::get()->getSettingValue<float>("creatorTextColorR");
            float g = Mod::get()->getSettingValue<float>("creatorTextColorG");
            float b = Mod::get()->getSettingValue<float>("creatorTextColorB");
            extraLabel->setColor({r, g, b});

            extraLabel->setPosition(xPosition, username->getPositionY());
            bg->addChild(extraLabel);

            xPosition += extraLabel->getContentSize().width + gap;
        }

        // Icon preview
        std::string choice = Mod::get()->getSettingValue<std::string>("creatorIcon");
        std::string spriteName = "GJ_creatorBtn_001.png";
        if (choice == "Star Icon") spriteName = "GJ_starsIcon_001.png";
        else if (choice == "Like Button") spriteName = "GJ_likeBtn_001.png";
        else if (choice == "Gold Star") spriteName = "GJ_goldStar_001.png";

        auto icon = CCSprite::createWithSpriteFrameName(spriteName.c_str());
        if (icon) {
            float maxScale = Mod::get()->getSettingValue<float>("maxIconScale");
            icon->setScale(maxScale);
            icon->setAnchorPoint({0.f, 0.5f});
            icon->setPosition(xPosition, username->getPositionY());
            bg->addChild(icon);
        }
    });
}

class $modify(MyCommentCell, CommentCell) {
    bool init(Comment* comment) {
        if (!CommentCell::init(comment))
            return false;

        if (!comment->m_hasCreatorLike) 
            return true;

        std::string choice = Mod::get()->getSettingValue<std::string>("creatorIcon");
        bool showText = Mod::get()->getSettingValue<bool>("showCreatorText");
        float gap = Mod::get()->getSettingValue<float>("textIconGap");
        float maxScale = Mod::get()->getSettingValue<float>("maxIconScale");

        float r = Mod::get()->getSettingValue<float>("creatorTextColorR");
        float g = Mod::get()->getSettingValue<float>("creatorTextColorG");
        float b = Mod::get()->getSettingValue<float>("creatorTextColorB");

        std::string spriteName;
        if (choice == "Star Icon") spriteName = "GJ_starsIcon_001.png";
        else if (choice == "Like Button") spriteName = "GJ_likeBtn_001.png";
        else if (choice == "Gold Star") spriteName = "GJ_goldStar_001.png";
        else spriteName = "GJ_creatorBtn_001.png";

        float xPosition = 0.0f;

        if (showText && m_authorLabel) {
            auto extraLabel = CCLabelBMFont::create("(Liked by Creator)", "chatFont.fnt");
            extraLabel->setScale(0.5f);
            extraLabel->setAnchorPoint({0.f, 0.5f});
            extraLabel->setColor({r, g, b});

            xPosition = m_authorLabel->getPositionX() + m_authorLabel->getContentSize().width + gap;
            extraLabel->setPosition(xPosition, m_authorLabel->getPositionY());
            m_mainLayer->addChild(extraLabel);

            xPosition += extraLabel->getContentSize().width + gap;
        } else if (m_authorLabel) {
            xPosition = m_authorLabel->getPositionX() + m_authorLabel->getContentSize().width + gap;
        }

        if (m_likeSprite) { 
            auto creatorIcon = CCSprite::createWithSpriteFrameName(spriteName.c_str());
            if (creatorIcon) {
                creatorIcon->setAnchorPoint({0.f, 0.5f});
                float maxX = m_likeSprite->getPositionX() - gap;
                float availableWidth = maxX - xPosition;

                float scale = maxScale;
                if (creatorIcon->getContentSize().width * scale > availableWidth) {
                    scale = availableWidth / creatorIcon->getContentSize().width;
                    if (scale < 0.1f) scale = 0.1f;
                }

                creatorIcon->setScale(scale);
                creatorIcon->setPosition(xPosition, m_likeSprite->getPositionY());
                m_mainLayer->addChild(creatorIcon);
            }
        }

        return true;
    }
};
