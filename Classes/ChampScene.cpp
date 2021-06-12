#include "ChampScene.h"

using namespace cocos2d;
using namespace experimental;

Scene* Champ::createScene() {
    return Champ::create();
}

bool Champ::init() {
    if (!Scene::init())    {
        return false;
    }
	
	this->setCascadeOpacityEnabled(true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto keyboardListener = EventListenerKeyboard::create();

	if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
		UserDefault::getInstance()->setIntegerForKey("backgroundMusic", AudioEngine::play2d("champ.mp3", true));
		UserDefault::getInstance()->flush();
		AudioEngine::play2d("select.ogg", false);
	}

	auto archerFrames = getAnimation("archerAlert", 10);
	auto archer = Sprite::createWithSpriteFrame(archerFrames.front());
	archer->setTag(0);
	archer->setPosition(Vec2(visibleSize.width/4, visibleSize.height/3));
	archer->setScale(0.5f);
	auto archerAnimation = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(archerFrames, 1.0f/10)));
	archer->runAction(archerAnimation);
	this->addChild(archer);

	auto wizardFrames = getAnimation("wizardAlert", 10);
	auto wizard = Sprite::createWithSpriteFrame(wizardFrames.front());
	wizard->setTag(1);
	wizard->setPosition(Vec2(visibleSize.width/2, visibleSize.height/3));
	wizard->setScale(0.5f);
	auto wizardAnimation = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(wizardFrames, 1.0f/10)));
	wizard->runAction(wizardAnimation);
	this->addChild(wizard);

	auto knightFrames = getAnimation("knightAlert", 10);
	auto knight = Sprite::createWithSpriteFrame(knightFrames.front());
	knight->setTag(2);
	knight->setPosition(Vec2(visibleSize.width*3/4, visibleSize.height/3));
	knight->setScale(0.5f);
	auto knightAnimation = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(knightFrames, 1.0f/10)));
	knight->runAction(knightAnimation);
	this->addChild(knight);

	auto one = Sprite::create("one.png");
	one->setTag(100);
	one->setName("0");
	one->setAnchorPoint(Vec2(3.0f, -10.0f));
	one->setPosition(this->getChildByTag(0)->getPosition());
	this->addChild(one);

	auto two = Sprite::create("two.png");
	two->setTag(200);
	two->setName("0");
	two->setAnchorPoint(Vec2(-3.0f, -10.0f));
	two->setPosition(this->getChildByTag(0)->getPosition());
	this->addChild(two);

	keyboardListener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch(keyCode) {
			case EventKeyboard::KeyCode::KEY_A:
				_playerOne.push_back(-1);
				break;
			case EventKeyboard::KeyCode::KEY_D:
				_playerOne.push_back(1);
				break;
			/*case EventKeyboard::KeyCode::KEY_W:
				_playerOne.push_back(-3); //number per row
				break;
			case EventKeyboard::KeyCode::KEY_S:
				_playerOne.push_back(3);
				break;*/
			case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
				_playerTwo.push_back(-1);
				break;
			case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
				_playerTwo.push_back(1);
				break;
			/*case EventKeyboard::KeyCode::KEY_UP_ARROW:
				_playerTwo.push_back(-3);
				break;
			case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
				_playerTwo.push_back(3);
				break;*/
			default:
				break;
		}
	};

	keyboardListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch(keyCode) {
			case EventKeyboard::KeyCode::KEY_A:
				_playerOne.erase(remove(_playerOne.begin(), _playerOne.end(), -1), _playerOne.end());
				break;
			case EventKeyboard::KeyCode::KEY_D:
				_playerOne.erase(remove(_playerOne.begin(), _playerOne.end(), 1), _playerOne.end());
				break;
			/*case EventKeyboard::KeyCode::KEY_W:
				_playerOne.erase(remove(_playerOne.begin(), _playerOne.end(), -3), _playerOne.end());
				break;
			case EventKeyboard::KeyCode::KEY_S:
				_playerOne.erase(remove(_playerOne.begin(), _playerOne.end(), 3), _playerOne.end());
				break;*/
			case EventKeyboard::KeyCode::KEY_F:
				_lockedOne = true;
				{
					auto ready = Label::createWithTTF("ready player one", "tribal.ttf", 20);
					ready->setPosition(Vec2(Director::getInstance()->getVisibleSize().width/4, Director::getInstance()->getVisibleSize().height*2/3));
					this->addChild(ready);
				}
				break;
			case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
				_playerTwo.erase(remove(_playerTwo.begin(), _playerTwo.end(), -1), _playerTwo.end());
				break;
			case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
				_playerTwo.erase(remove(_playerTwo.begin(), _playerTwo.end(), 1), _playerTwo.end());
				break;
			/*case EventKeyboard::KeyCode::KEY_UP_ARROW:
				_playerTwo.erase(remove(_playerTwo.begin(), _playerTwo.end(), -3), _playerTwo.end());
				break;
			case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
				_playerTwo.erase(remove(_playerTwo.begin(), _playerTwo.end(), 3), _playerTwo.end());
				break;*/
			case EventKeyboard::KeyCode::KEY_SLASH:
				_lockedTwo = true;
				{
					auto ready = Label::createWithTTF("ready player two", "tribal.ttf", 20);
					ready->setPosition(Vec2(Director::getInstance()->getVisibleSize().width*3/4, Director::getInstance()->getVisibleSize().height*2/3));
					this->addChild(ready);
				}
				break;
			case 164/*EventKeyboard::KeyCode::KEY_RETURN*/:
				if (_lockedOne && _lockedTwo) {
					UserDefault::getInstance()->setStringForKey("one", this->getChildByTag(100)->getName());
					UserDefault::getInstance()->setStringForKey("two", this->getChildByTag(200)->getName());
					UserDefault::getInstance()->flush();
					_eventDispatcher->removeEventListenersForTarget(this);
					if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
						AudioEngine::play2d("prep.ogg", false);
					}
					auto action = Sequence::create(
									Spawn::createWithTwoActions(
										FadeOut::create(1.5f),
										MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true)
									),
									CallFunc::create([&](){
										Director::getInstance()->replaceScene(Play::createScene());
									}),
									nullptr);
					this->runAction(action);
				}
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

void Champ::update(float dt) {
	if (!_playerOne.empty() && !_lockedOne && !_cooldownOne) {
		_cooldownOne = true;
		move(100, _playerOne.back());
	}
	if (!_playerTwo.empty() && !_lockedTwo && !_cooldownTwo) {
		_cooldownTwo = true;
		move(200, _playerTwo.back());
	}
	if (_lockedOne && _lockedTwo) {
		if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
			AudioEngine::play2d("ready.ogg", false);
		}
		this->unscheduleUpdate();
	}
}

void Champ::move(int playerNumber, int change) {
	auto sprite = this->getChildByTag(playerNumber);
	auto tag = (std::atoi(sprite->getName().c_str())+change+3)%3; //total number of champs
	sprite->setPosition(this->getChildByTag(tag)->getPosition());
	sprite->setName(std::to_string(tag));

	auto action = Sequence::create(
						DelayTime::create(0.2f),
						CallFunc::create([=](){
							if (playerNumber == 100) {
								_cooldownOne = false;
							} else {
								_cooldownTwo = false;
							}
						}),
						nullptr);
	this->runAction(action);
}

Vector<SpriteFrame*> Champ::getAnimation(std::string format, int count) {
	Vector<SpriteFrame*> animFrames;
	for(int i=0; i<count; i++) {
		animFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(format+std::to_string(i)+".png"));
	}
	return animFrames;
}