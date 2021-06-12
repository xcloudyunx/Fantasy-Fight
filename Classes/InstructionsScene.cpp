#include "InstructionsScene.h"

using namespace cocos2d;
using namespace experimental;

Scene* Instructions::createScene() {
    return Instructions::create();
}

bool Instructions::init() {
    if (!Scene::init())    {
        return false;
    }
	
	this->setCascadeOpacityEnabled(true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto keyboardListener = EventListenerKeyboard::create();

	if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
		UserDefault::getInstance()->setIntegerForKey("backgroundMusic", AudioEngine::play2d("home.mp3", true));
		UserDefault::getInstance()->flush();
	}
	
	auto title = Label::createWithTTF("Instructions", "tribal.ttf", 50);
	title->setPosition(Vec2(visibleSize.width/2, visibleSize.height*3/4));
	this->addChild(title);
	
	auto player1 = Label::createWithTTF("player one", "tribal.ttf", 35);
	player1->setPosition(Vec2(visibleSize.width/4, visibleSize.height/2));
	this->addChild(player1);
	
	auto moves1 = Label::createWithTTF("move: wasd\n\nattack: f\n\nspecial: g", "tribal.ttf", 20);
	moves1->setPosition(Vec2(visibleSize.width/4, visibleSize.height*3/10));
	this->addChild(moves1);
	
	auto player2 = Label::createWithTTF("player two", "tribal.ttf", 35);
	player2->setPosition(Vec2(visibleSize.width*3/4, visibleSize.height/2));
	this->addChild(player2);
	
	auto moves2 = Label::createWithTTF("move: arrows\n\nattack: /\n\nspecial: .", "tribal.ttf", 20);
	moves2->setPosition(Vec2(visibleSize.width*3/4, visibleSize.height*3/10));
	this->addChild(moves2);

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