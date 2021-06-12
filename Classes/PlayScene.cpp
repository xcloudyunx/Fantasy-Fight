#include "PlayScene.h"

using namespace cocos2d;
using namespace experimental;

Scene* Play::createScene() {
	auto scene = Scene::createWithPhysics();
	auto world = scene->getPhysicsWorld();
	world->setGravity(Vec2(0.0f, -1962.0f));
	//world->setDebugDrawMask( PhysicsWorld::DEBUGDRAW_ALL );
	auto layer = Play::create();
	scene->addChild(layer);
	return scene;
}

bool Play::init() {
	if (!Scene::init()) {
		return false;
	}
	
	this->setCascadeOpacityEnabled(true);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto keyboardListener = EventListenerKeyboard::create();
	auto contactListener = EventListenerPhysicsContact::create();

	if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
		UserDefault::getInstance()->setIntegerForKey("backgroundMusic", AudioEngine::play2d("play.mp3", true));
		UserDefault::getInstance()->flush();
		AudioEngine::play2d("fight.ogg", false);
	}
	
	auto pauseBackground = Sprite::create("pauseBackground.png");
	pauseBackground->setPosition(visibleSize/2);
	pauseBackground->setName("pauseBackground");
	pauseBackground->setGlobalZOrder(1.0f);
	pauseBackground->setVisible(false);
	pauseBackground->setCascadeOpacityEnabled(true);
	this->addChild(pauseBackground);
	
	auto title = Label::createWithTTF("paused", "tribal.ttf", 50);
	title->setPosition(Vec2(visibleSize.width/2, visibleSize.height*3/4));
	title->setGlobalZOrder(1.0f);
	pauseBackground->addChild(title);
	
	auto resume = Label::createWithTTF("resume", "tribal.ttf", 20);
	resume->setTag(0);
	resume->setPosition(visibleSize/2);
	resume->setGlobalZOrder(1.0f);
	pauseBackground->addChild(resume);
	
	auto restart = Label::createWithTTF("restart", "tribal.ttf", 20);
	restart->setTag(1);
	restart->setPosition(Vec2(visibleSize.width/2, visibleSize.height/3));
	restart->setGlobalZOrder(1.0f);
	pauseBackground->addChild(restart);
	
	auto home = Label::createWithTTF("home", "tribal.ttf", 20);
	home->setTag(2);
	home->setPosition(Vec2(visibleSize.width/2, visibleSize.height/6));
	home->setGlobalZOrder(1.0f);
	pauseBackground->addChild(home);
	
	auto marker = Sprite::create("marker.png");
	marker->setTag(100);
	marker->setName("0");
	marker->setPosition(pauseBackground->getChildByTag(0)->getPosition());
	marker->setGlobalZOrder(1.0f);
	pauseBackground->addChild(marker);
	
	auto background = Sprite::create("background"+std::to_string(rand()%2+1)+".png");
	background->setScale(14);
	background->setPosition(visibleSize/2);
	this->addChild(background);

	auto border = PhysicsBody::createEdgeBox(Size(2100, 1200), PhysicsMaterial(0.0f, 0.0f, 0.0f), 100);
	border->setDynamic(false);
	border->setCategoryBitmask(0b10000);
	border->setCollisionBitmask(0b00011);
	border->setContactTestBitmask(0xFFFFFFFF);
	auto map = cocos2d::TMXTiledMap::create("map.tmx");
	map->setName("map");
	map->addComponent(border);
	this->addChild(map);
	for (auto object : map->getObjectGroup("Ground")->getObjects()) {
		auto properties = object.asValueMap();
		auto physicsBody = PhysicsBody::createBox(Size(properties.at("width").asInt(), properties.at("height").asInt()), PhysicsMaterial(0.0f, 0.0f, 1.0f));
		physicsBody->setDynamic(false);
		physicsBody->setCategoryBitmask(0b01000);
		physicsBody->setCollisionBitmask(0b01101);
		physicsBody->setContactTestBitmask(0xFFFFFFFF);
		auto node = Node::create();
		node->setPosition(properties.at("x").asFloat()+properties.at("width").asInt()/2, properties.at("y").asFloat()+properties.at("height").asInt()/2);
		node->addComponent(physicsBody);
		this->addChild(node);
	}

	std::map<std::string, std::string> m = {{"0", "archer"}, {"1", "wizard"}, {"2", "knight"}};

	_playerOne = new Player(1, m.at(UserDefault::getInstance()->getStringForKey("one")), Vec2(500, 500), this);
	_playerTwo = new Player(2, m.at(UserDefault::getInstance()->getStringForKey("two")), Vec2(1420, 500), this);

	keyboardListener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		if (_paused) {
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
		} else {
			switch(keyCode) {
				case EventKeyboard::KeyCode::KEY_A:
					_playerOne->pressLeft();
					break;
				case EventKeyboard::KeyCode::KEY_D:
					_playerOne->pressRight();
					break;
				case EventKeyboard::KeyCode::KEY_W:
					_playerOne->pressUp();
					break;
				case EventKeyboard::KeyCode::KEY_F:
					_playerOne->pressAttack();
					break;
				case EventKeyboard::KeyCode::KEY_G:
					_playerOne->special();
					break;
				case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
					_playerTwo->pressLeft();
					break;
				case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
					_playerTwo->pressRight();
					break;
				case EventKeyboard::KeyCode::KEY_UP_ARROW:
					_playerTwo->pressUp();
					break;
				case EventKeyboard::KeyCode::KEY_SLASH:
					_playerTwo->pressAttack();
					break;
				case EventKeyboard::KeyCode::KEY_PERIOD:
					_playerTwo->special();
					break;

				default:
					break;
			}
		}
	};

	keyboardListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		if (_paused) {
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
					if (this->getChildByName("pauseBackground")->getChildByTag(100)->getName() == "0") {
						Director::getInstance()->getActionManager()->resumeTargets(_pausedActions);
						Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1);
						AudioEngine::resumeAll();
						_paused = false;
						this->getChildByName("pauseBackground")->setVisible(false);
					} else {
						_eventDispatcher->removeEventListenersForTarget(this);
						cFade(2.0f);
						this->runAction(Sequence::create(
											Spawn::createWithTwoActions(
												FadeOut::create(1.5f),
												MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true)
											),
											CallFunc::create([&](){
												if (this->getChildByName("pauseBackground")->getChildByTag(100)->getName() == "1") {
													Director::getInstance()->replaceScene(Play::createScene());
												} else if (this->getChildByName("pauseBackground")->getChildByTag(100)->getName() == "2") {
													Director::getInstance()->popScene();
												}
											}),
											nullptr));
					}
					break;
				case EventKeyboard::KeyCode::KEY_ESCAPE:
					_eventDispatcher->removeEventListenersForTarget(this);
					cFade(2.0f);
					this->runAction(Sequence::create(
										MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true),
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
		} else {
			switch(keyCode) {
				case EventKeyboard::KeyCode::KEY_A:
					_playerOne->stopLeft();
					break;
				case EventKeyboard::KeyCode::KEY_D:
					_playerOne->stopRight();
					break;
				case EventKeyboard::KeyCode::KEY_W:
					_playerOne->stopUp();
					break;
				case EventKeyboard::KeyCode::KEY_F:
					_playerOne->stopAttack();
					break;
				case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
					_playerTwo->stopLeft();
					break;
				case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
					_playerTwo->stopRight();
					break;
				case EventKeyboard::KeyCode::KEY_UP_ARROW:
					_playerTwo->stopUp();
					break;
				case EventKeyboard::KeyCode::KEY_SLASH:
					_playerTwo->stopAttack();
					break;
				case EventKeyboard::KeyCode::KEY_ESCAPE:
					_pausedActions = Director::getInstance()->getActionManager()->pauseAllRunningActions();
					Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(0);
					AudioEngine::pauseAll();
					_paused = true;
					this->getChildByName("pauseBackground")->setVisible(true);
					break;
				default:
					break;
			}
		}
	};

	contactListener->onContactBegin = [&](PhysicsContact& contact) {
		auto a = contact.getShapeA()->getBody();
		auto b = contact.getShapeB()->getBody();
		auto aCat = a->getCategoryBitmask();
		auto bCat = b->getCategoryBitmask();
		auto aCol = a->getCollisionBitmask();
		auto bCol = b->getCollisionBitmask();

		if ((aCat & bCol) == 0b10000 || (bCat & aCol) == 0b10000) {
			if ((aCat & bCol) == 0b00010 || (bCat & aCol) == 0b00010) {
				if (aCat == 0b00010 && a->getTag() != 2) {
					a->getNode()->removeFromParent();
				} else if (b->getTag() != 2) {
					b->getNode()->removeFromParent();
				}
				return false;
			} else {
				return true;
			}
		} else if ((aCat & bCol) == 0b01000 || (bCat & aCol) == 0b01000) {
			if (a->getGroup() == -1 || b->getGroup() == -1) {
				_playerOne->touchingGround = true;
			} else if (a->getGroup() == -2 || b->getGroup() == -2) {
				_playerTwo->touchingGround = true;
			}
			return true;
		} else if ((aCat & bCol) == 0b00010 || (bCat & aCol) == 0b00010) {
			if (aCat == 0b00010) {
				if (a->getGroup() == -1) {
					_playerTwo->hit(_playerOne->damage);
				} else {
					_playerOne->hit(_playerTwo->damage);
				}
				if (a->getTag() != 2) {
					a->getNode()->removeFromParent();
				}
				if (a->getTag() == 1) {
					return true;
				} else {
					return false;
				}
			} else {
				if (b->getGroup() == -1) {
					_playerTwo->hit(_playerOne->damage);
				} else {
					_playerOne->hit(_playerTwo->damage);
				}
				if (b->getTag() != 2) {
					b->getNode()->removeFromParent();
				}
				if (b->getTag() == 1) {
					return true;
				} else {
					return false;
				}
			}
		}

		return false;
	};

	contactListener->onContactSeparate = [&](PhysicsContact& contact) {
		auto a = contact.getShapeA()->getBody();
		auto b = contact.getShapeB()->getBody();
		auto aCat = a->getCategoryBitmask();
		auto bCat = b->getCategoryBitmask();
		auto aCol = a->getCollisionBitmask();
		auto bCol = b->getCollisionBitmask();

		if ((aCat & bCol) == 0b1000 || (bCat & aCol) == 0b1000) {
			if (a->getGroup() == -1 || b->getGroup() == -1) {
				_playerOne->touchingGround = false;
			} else if (a->getGroup() == -2 || b->getGroup() == -2) {
				_playerTwo->touchingGround = false;
			}
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
	this->scheduleUpdate();

    return true;
}

void Play::update(float dt) {
	if (_paused) {
		if (!_marker.empty() && !_cooldown) {
			_cooldown = true;
			move(_marker.back());
		}
	} else {
		_playerOne->update();
		_playerTwo->update();
	}
}

void Play::move(int change) {
	auto sprite = this->getChildByName("pauseBackground")->getChildByTag(100);
	auto tag = (std::atoi(sprite->getName().c_str())+change+3)%3; //total number of options
	sprite->setPosition(this->getChildByName("pauseBackground")->getChildByTag(tag)->getPosition());
	sprite->setName(std::to_string(tag));

	auto action = Sequence::create(
						DelayTime::create(0.2f),
						CallFunc::create([=](){
							_cooldown = false;
						}),
						nullptr);
	this->getChildByName("pauseBackground")->runAction(action);
}

void Play::cFade(float duration) {
	this->runAction(FadeOut::create(duration));
	auto m = static_cast<cocos2d::TMXTiledMap*>(this->getChildByName("map"));
	auto l = m->getLayer("Ground");
	for (int i=0; i<l->getLayerSize().width; i++) {
		for (int j=30; j<l->getLayerSize().height; j++) {
			l->getTileAt(Vec2(i, j))->runAction(FadeOut::create(duration));
		}
	}
}