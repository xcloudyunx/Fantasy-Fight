#include "HomeScene.h"

using namespace cocos2d;
using namespace experimental;
using namespace ui;

Scene* Home::createScene() {
    return Home::create();
}

bool Home::init() {
    if (!Scene::init()) {
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
	
	auto title = Label::createWithTTF("fantasy fight", "tribal.ttf", 50);
	title->setPosition(Vec2(visibleSize.width/2, visibleSize.height*3/4));
	this->addChild(title);
	
	auto start = Label::createWithTTF("start", "tribal.ttf", 20);
	start->setTag(0);
	start->setPosition(visibleSize/2);
	this->addChild(start);
	
	auto settings = Label::createWithTTF("settings", "tribal.ttf", 20);
	settings->setTag(1);
	settings->setPosition(Vec2(visibleSize.width/2, visibleSize.height*3/8));
	this->addChild(settings);
	
	auto instructions = Label::createWithTTF("instructions", "tribal.ttf", 20);
	instructions->setTag(2);
	instructions->setPosition(Vec2(visibleSize.width/2, visibleSize.height/4));
	this->addChild(instructions);
	
	auto credits = Label::createWithTTF("credits", "tribal.ttf", 20);
	credits->setTag(3);
	credits->setPosition(Vec2(visibleSize.width/2, visibleSize.height/8));
	this->addChild(credits);
	
	auto marker = Sprite::create("marker.png");
	marker->setTag(100);
	marker->setName("0");
	marker->setAnchorPoint(Vec2(11.0f, 0.5f));
	marker->setPosition(this->getChildByTag(0)->getPosition());
	this->addChild(marker);
	
	keyboardListener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch(keyCode) {
			case EventKeyboard::KeyCode::KEY_UP_ARROW:
				_marker.push_back(-1);
				break;
			case EventKeyboard::KeyCode::KEY_W:
				_marker.push_back(-1);
				break;
			case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
				_marker.push_back(1);
				break;
			case EventKeyboard::KeyCode::KEY_S:
				_marker.push_back(1);
				break;
			default:
				break;
		}
	};
	
	keyboardListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch(keyCode) {
			case EventKeyboard::KeyCode::KEY_UP_ARROW:
				_marker.erase(remove(_marker.begin(), _marker.end(), -1), _marker.end());
				break;
			case EventKeyboard::KeyCode::KEY_W:
				_marker.erase(remove(_marker.begin(), _marker.end(), -1), _marker.end());
				break;
			case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
				_marker.erase(remove(_marker.begin(), _marker.end(), 1), _marker.end());
				break;
			case EventKeyboard::KeyCode::KEY_S:
				_marker.erase(remove(_marker.begin(), _marker.end(), 1), _marker.end());
				break;
			case 164/*EventKeyboard::KeyCode::KEY_RETURN*/:
				_eventDispatcher->pauseEventListenersForTarget(this);
				this->runAction(Sequence::create(
									Spawn::createWithTwoActions(
										FadeOut::create(1.5f),
										MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true)
									),
									CallFunc::create([&](){
										if (this->getChildByTag(100)->getName() == "0") {
											Director::getInstance()->pushScene(Champ::createScene());
										} else if (this->getChildByTag(100)->getName() == "1") {
											Director::getInstance()->pushScene(Settings::createScene());
										} else if (this->getChildByTag(100)->getName() == "2") {
											Director::getInstance()->pushScene(Instructions::createScene());
										} else if (this->getChildByTag(100)->getName() == "3") {
											Director::getInstance()->pushScene(Credits::createScene());
										}
										this->setOpacity(255);
										_eventDispatcher->resumeEventListenersForTarget(this);
									}),
									nullptr));
				break;
			case EventKeyboard::KeyCode::KEY_ESCAPE:
				this->runAction(Sequence::create(
									Spawn::createWithTwoActions(
										FadeOut::create(1.5f),
										MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true)
									),
									CallFunc::create([&](){
										Director::getInstance()->end();
									}),
									nullptr));
				break;
			default:
				break;
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	this->scheduleUpdate();

	return true;
}

void Home::update(float dt) {
	if (!_marker.empty() && !_cooldown) {
		_cooldown = true;
		move(_marker.back());
	}
}

void Home::move(int change) {
	auto sprite = this->getChildByTag(100);
	auto tag = (std::atoi(sprite->getName().c_str())+change+4)%4; //total number of options
	sprite->setPosition(this->getChildByTag(tag)->getPosition());
	sprite->setName(std::to_string(tag));

	auto action = Sequence::create(
						DelayTime::create(0.2f),
						CallFunc::create([=](){
							_cooldown = false;
						}),
						nullptr);
	this->runAction(action);
}