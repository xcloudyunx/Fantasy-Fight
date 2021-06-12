#include "SettingsScene.h"

using namespace cocos2d;
using namespace experimental;

Scene* Settings::createScene() {
    return Settings::create();
}

bool Settings::init() {
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
	
	auto title = Label::createWithTTF("settings", "tribal.ttf", 50);
	title->setPosition(Vec2(visibleSize.width/2, visibleSize.height*3/4));
	this->addChild(title);
	
	auto sound = Label::createWithTTF("sound", "tribal.ttf", 20);
	sound->setTag(0);
	sound->setPosition(visibleSize/2);
	if (UserDefault::getInstance()->getBoolForKey("mute", false)) {
		sound->setOpacity(150);
	}
	this->addChild(sound);
	
	auto screen = Label::createWithTTF("full screen", "tribal.ttf", 20);
	screen->setTag(1);
	screen->setPosition(Vec2(visibleSize.width/2, visibleSize.height/3));
	if (!UserDefault::getInstance()->getBoolForKey("fullscreen", true)) {
		screen->setOpacity(150);
	}
	this->addChild(screen);
	
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
				if (this->getChildByTag(100)->getName() == "0") {
					if (this->getChildByTag(0)->getOpacity() == 150) {
						this->getChildByTag(0)->setOpacity(255);
						UserDefault::getInstance()->setBoolForKey("mute", false);
						UserDefault::getInstance()->setIntegerForKey("backgroundMusic", AudioEngine::play2d("home.mp3", true));
					} else {
						this->getChildByTag(0)->setOpacity(150);
						AudioEngine::stopAll();
						UserDefault::getInstance()->setBoolForKey("mute", true);
					}
				} else if (this->getChildByTag(100)->getName() == "1") {
					auto glview = Director::getInstance()->getOpenGLView();
					if (this->getChildByTag(1)->getOpacity() == 150) {
						this->getChildByTag(1)->setOpacity(255);
						UserDefault::getInstance()->setBoolForKey("fullscreen", true);
						dynamic_cast<GLViewImpl*>(glview)->setFullscreen();
					} else {
						this->getChildByTag(1)->setOpacity(150);
						UserDefault::getInstance()->setBoolForKey("fullscreen", false);
						dynamic_cast<GLViewImpl*>(glview)->setWindowed(1600, 900);
					}
				}
				
				UserDefault::getInstance()->flush();
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

void Settings::update(float dt) {
	if (!_marker.empty() && !_cooldown) {
		_cooldown = true;
		move(_marker.back());
	}
}

void Settings::move(int change) {
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