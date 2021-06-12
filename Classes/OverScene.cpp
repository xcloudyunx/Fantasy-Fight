#include "OverScene.h"
#include "ChampScene.h"

using namespace cocos2d;
using namespace experimental;

Scene* Over::createScene() {
    return Over::create();
}

bool Over::init() {
    if (!Scene::init())    {
        return false;
    }
	
	this->setCascadeOpacityEnabled(true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto keyboardListener = EventListenerKeyboard::create();

	if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
		UserDefault::getInstance()->setIntegerForKey("backgroundMusic", AudioEngine::play2d("over.mp3", true));
		UserDefault::getInstance()->flush();
	}
	
	auto title = Label::createWithTTF("game over", "tribal.ttf", 50);
	title->setPosition(Vec2(visibleSize.width/2, visibleSize.height*3/4));
	this->addChild(title);
	
	std::string win = "1";
	Color4B c = Color4B(100, 255, 100, 255);
	if (UserDefault::getInstance()->getIntegerForKey("loser") == 1) {
		win = "2";
		c = Color4B(255, 100, 100, 255);
	}
	auto winner = Label::createWithTTF("player "+win+" wins!", "tribal.ttf", 30);
	winner->setTextColor(c);
	winner->setPosition(Vec2(visibleSize.width/2, visibleSize.height*3/5));
	this->addChild(winner);
	
	auto home = Label::createWithTTF("home", "tribal.ttf", 20);
	home->setTag(0);
	home->setPosition(visibleSize.width/2, visibleSize.height*4/9);
	this->addChild(home);
	
	auto restart = Label::createWithTTF("restart", "tribal.ttf", 20);
	restart->setTag(1);
	restart->setPosition(Vec2(visibleSize.width/2, visibleSize.height/3));
	this->addChild(restart);
	
	auto marker = Sprite::create("marker.png");
	marker->setTag(100);
	marker->setName("0");
	marker->setAnchorPoint(Vec2(9.0f, 0.5f));
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
				_eventDispatcher->removeEventListenersForTarget(this);
				this->runAction(Sequence::create(
									Spawn::createWithTwoActions(
										FadeOut::create(1.5f),
										MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true)
									),
									CallFunc::create([&](){
										if (this->getChildByTag(100)->getName() == "0") {
											if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
												UserDefault::getInstance()->setIntegerForKey("backgroundMusic", AudioEngine::play2d("home.mp3", true));
												UserDefault::getInstance()->flush();
											}
											Director::getInstance()->popScene();
										} else if (this->getChildByTag(100)->getName() == "1") {
											Director::getInstance()->replaceScene(Champ::createScene());
										}
									}),
									nullptr));
				break;
			case EventKeyboard::KeyCode::KEY_R:
				_eventDispatcher->removeEventListenersForTarget(this);
				this->runAction(Sequence::create(
									Spawn::createWithTwoActions(
										FadeOut::create(1.5f),
										MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true)
									),
									CallFunc::create([&](){
										Director::getInstance()->replaceScene(Champ::createScene());
									}),
									nullptr));
				break;
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
	this->scheduleUpdate();

	return true;
}

void Over::update(float dt) {
	if (!_marker.empty() && !_cooldown) {
		_cooldown = true;
		move(_marker.back());
	}
}

void Over::move(int change) {
	auto sprite = this->getChildByTag(100);
	auto tag = (std::atoi(sprite->getName().c_str())+change+2)%2; //total number of options
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