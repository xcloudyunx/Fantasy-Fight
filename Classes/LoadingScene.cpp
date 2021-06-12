#include "LoadingScene.h"

using namespace cocos2d;
using namespace experimental;

Scene* Loading::createScene() {
    return Loading::create();
}

bool Loading::init() {
    if (!Scene::init())    {
        return false;
    }
	
	this->setCascadeOpacityEnabled(true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto logo = Sprite::create("logo.png");
	logo->setPosition(visibleSize/2);
	this->addChild(logo);

	auto sprites = SpriteFrameCache::getInstance();

	//preload audio
	AudioEngine::preload("home.mp3");
	AudioEngine::preload("select.mp3");
	AudioEngine::preload("play.mp3");
	AudioEngine::preload("finish.mp3");

	//preload files
	sprites->addSpriteFramesWithFile("archer.plist");
	sprites->addSpriteFramesWithFile("wizard.plist");
	sprites->addSpriteFramesWithFile("knight.plist");

	auto transition = Sequence::create(
						FadeOut::create(2.0f),
						//FadeOut::create(0.1f),
						CallFunc::create([&](){
							Director::getInstance()->replaceScene(Home::createScene());
						}),
						nullptr);
	this->runAction(transition);

	return true;
}