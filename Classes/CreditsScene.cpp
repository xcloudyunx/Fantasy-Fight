#include "CreditsScene.h"

using namespace cocos2d;
using namespace experimental;

Scene* Credits::createScene() {
    return Credits::create();
}

bool Credits::init() {
    if (!Scene::init())    {
        return false;
    }
	
	this->setCascadeOpacityEnabled(true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto keyboardListener = EventListenerKeyboard::create();

	if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
		UserDefault::getInstance()->setIntegerForKey("backgroundMusic", AudioEngine::play2d("credits.mp3", true));
		UserDefault::getInstance()->flush();
	}
	
	auto title = Label::createWithTTF("credits", "tribal.ttf", 50);
	title->setPosition(Vec2(visibleSize.width/2, visibleSize.height*3/4));
	this->addChild(title);
	
	auto game = Label::createWithTTF(/*"programming by yunge yu - www.yungeyu.com"*/"programming by yunge yu", "tribal.ttf", 20);
	//Application::getInstance()->openURL("http://cloudgalley.com");
	game->setPosition(visibleSize/2);
	this->addChild(game);
	
	auto music = Label::createWithTTF("music & sound effects by eric matyas - www.soundimage.org", "tribal.ttf", 20);
	music->setPosition(Vec2(visibleSize.width/2, visibleSize.height/4));
	this->addChild(music);

	keyboardListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch(keyCode) {
			case EventKeyboard::KeyCode::KEY_ESCAPE:
				_eventDispatcher->removeEventListenersForTarget(this);
				this->runAction(Sequence::create(
									Spawn::createWithTwoActions(
										FadeOut::create(1.5f),
										MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true)
									),
									CallFunc::create([&](){
										if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
											UserDefault::getInstance()->setIntegerForKey("backgroundMusic", AudioEngine::play2d("home.mp3", true));
											UserDefault::getInstance()->flush();
										}
										Director::getInstance()->popScene();
									}),
									nullptr));
				break;
			default:
				break;
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}