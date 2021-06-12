#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include "AudioEngine.h"
#include "ui/CocosGUI.h"
#include "OverScene.h"
#include "MusicFade.h"

using namespace cocos2d;
using namespace experimental;
using namespace ui;

class Player {
	Scene* _this;

	std::string _playerNumber;
	int _pN;

	std::string _champion;

	std::vector<Sprite*> _actions;

	std::vector<std::string> _directions;
	std::vector<std::string> _singleAction;

	Vector<SpriteFrame*> _jumpFrames;
	Vector<SpriteFrame*> _attackFrames;
	Vector<SpriteFrame*> _hitFrames;
	Vector<SpriteFrame*> _dieFrames;

	bool _jumpCD = false;
	bool _attacking = false;
	bool _attackCD = false;
	bool _specialCD = false;
	bool _hit = false;
	bool _dead = false;

	int _health;
	int _speed;
	int _attackType;
	
	int _healthRemaining;
	LoadingBar* _healthBar;
	Sprite* _healthBase;

	Vector<SpriteFrame*> getAnimation(std::string format, int count) {
		Vector<SpriteFrame*> animFrames;
		for(int i=0; i<count; i++) {
			animFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(_champion+format+std::to_string(i)+".png"));
		}
		return animFrames;
	}

	void reset() {
		for (auto &sprite : _actions) {
			if (sprite->getName() == "alert"+_playerNumber) {
				sprite->setVisible(true);
				sprite->resume();
			} else {
				sprite->setVisible(false);
			}
			sprite->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
		}
	}

	void run(std::string direction) {
		for (auto &sprite : _actions) {
			if (direction == "left") {
				sprite->setScaleX(-0.5f);
				if (sprite->getName() == "alert"+_playerNumber) {
					sprite->getPhysicsBody()->setVelocity(Vec2(-_speed, sprite->getPhysicsBody()->getVelocity().y));
				}
			} else if (direction == "right") {
				sprite->setScaleX(0.5f);
				if (sprite->getName() == "alert"+_playerNumber) {
					sprite->getPhysicsBody()->setVelocity(Vec2(_speed, sprite->getPhysicsBody()->getVelocity().y));
				}
			}

			if (sprite->getName() == "run"+_playerNumber && touchingGround && !_attacking) {
				sprite->setVisible(true);
				sprite->resume();
			} else if (sprite->getName() == "jump"+_playerNumber) {
				if (touchingGround) {
					sprite->setVisible(false);
				} else {
					sprite->setVisible(true);
				}
			} else if (sprite->getName() == "attack"+_playerNumber) {
				if (_attacking) {
					sprite->setVisible(true);
				} else {
					sprite->setVisible(false);
				}
			} else {
				sprite->setVisible(false);
			}
			sprite->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
		}
	}

	void jump() {
		touchingGround = false;
		_jumpCD = true;
		for (auto &sprite : _actions) {
			if (sprite->getName() == "jump"+_playerNumber) {
				sprite->setVisible(true);
				auto jumpAnimation = Spawn::createWithTwoActions(
										Animate::create(Animation::createWithSpriteFrames(_jumpFrames, 1.0f/15)),
										Sequence::create(
											DelayTime::create(0.1f),
											CallFunc::create([&](){
												_this->getChildByName("alert"+_playerNumber)->getPhysicsBody()->setVelocity(Vec2(_this->getChildByName("alert"+_playerNumber)->getPhysicsBody()->getVelocity().x, 1000));
											}),
											DelayTime::create(0.2f),
											CallFunc::create([&](){
												_this->getChildByName("alert"+_playerNumber)->getPhysicsBody()->setPositionOffset(Vec2(0, 50));
											}),
											DelayTime::create(0.3f),
											CallFunc::create([&](){
												_this->getChildByName("alert"+_playerNumber)->getPhysicsBody()->setPositionOffset(Vec2(0, 0));
											}),
											DelayTime::create(0.3f),
											CallFunc::create([&](){
												_jumpCD = false;
											}),
										nullptr)
									);
				sprite->runAction(jumpAnimation);
			} else {
				sprite->setVisible(false);
			}
			sprite->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
		}
	}

	void attack() {
		_attacking = true;
		_attackCD = true;
		for (auto &sprite : _actions) {
			if (sprite->getName() == "attack"+_playerNumber) {
				sprite->setVisible(true);
				auto attackAnimation = Sequence::create(
											Animate::create(Animation::createWithSpriteFrames(_attackFrames, 1.0f/20)),
											CallFunc::create([&](){
												_attacking = false;
											}),
											nullptr);
				sprite->runAction(attackAnimation);
				if (_champion == "archer") {
					auto tmp = Sequence::create(
									DelayTime::create(0.5f),
									CallFunc::create([&](){
										auto physicsBody = PhysicsBody::createBox(Size(150, 30), PhysicsMaterial(0.0f, 0.0f, 1.0f));
										physicsBody->setGravityEnable(false);
										physicsBody->setCategoryBitmask(0b00010);
										physicsBody->setCollisionBitmask(0b10001);
										physicsBody->setContactTestBitmask(0xFFFFFFFF);
										physicsBody->setGroup(_this->getChildByName("alert"+_playerNumber)->getPhysicsBody()->getGroup());
										physicsBody->setVelocity(sprite->getScaleX()*Vec2(2000, 0));
										physicsBody->setRotationEnable(false);
										physicsBody->setTag(_attackType);

										auto arrow = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("Arrow.png"));
										arrow->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
										arrow->setScale(sprite->getScaleX()*1.5f);
										arrow->addComponent(physicsBody);
										_this->addChild(arrow);
									}),
									DelayTime::create(0.6f),
									CallFunc::create([&](){
										_attackCD = false;
									}),
									nullptr);
					sprite->runAction(tmp);
				} else if (_champion == "wizard") {
					auto tmp = Sequence::create(
									DelayTime::create(0.1f),
									CallFunc::create([&](){
										auto physicsBody = PhysicsBody::createBox(Size(0.1f, 200.0f), PhysicsMaterial(0.0f, 0.0f, 0.0f), Vec2(0, 100.0f));
										physicsBody->setGravityEnable(false);
										physicsBody->setCategoryBitmask(0b00010);
										physicsBody->setCollisionBitmask(0b10001);
										physicsBody->setContactTestBitmask(0xFFFFFFFF);
										physicsBody->setGroup(_this->getChildByName("alert"+_playerNumber)->getPhysicsBody()->getGroup());
										physicsBody->setVelocity(sprite->getScaleX()*Vec2(4500, 0));
										physicsBody->setRotationEnable(false);
										physicsBody->setTag(_attackType);

										auto spell = Node::create();
										spell->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
										spell->addComponent(physicsBody);
										spell->setTag(_pN*1000);
										_this->addChild(spell);

										auto removeSpell = Sequence::create(
																DelayTime::create(0.25f),
																CallFunc::create([&](){
																	_this->getChildByTag(_pN*1000)->removeFromParent();
																}),
																nullptr);
										spell->runAction(removeSpell);
									}),
									DelayTime::create(0.8f),
									CallFunc::create([&](){
										_attackCD = false;
									}),
									nullptr);
					sprite->runAction(tmp);
				} else if (_champion == "knight") {
					auto tmp = Sequence::create(
									DelayTime::create(0.20f),
									CallFunc::create([&](){
										auto physicsBody = PhysicsBody::createBox(Size(20.0f, 100.0f), PhysicsMaterial(0.0f, 0.0f, 1.0f), Vec2(sprite->getScaleX()*-50, 20.0f));
										physicsBody->setGravityEnable(false);
										physicsBody->setCategoryBitmask(0b00010);
										physicsBody->setCollisionBitmask(0b10001);
										physicsBody->setContactTestBitmask(0xFFFFFFFF);
										physicsBody->setGroup(_this->getChildByName("alert"+_playerNumber)->getPhysicsBody()->getGroup());
										physicsBody->setAngularVelocity(sprite->getScaleX()*-170);
										physicsBody->setTag(_attackType);

										auto sword = Node::create();
										sword->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
										sword->setTag(_pN*10000);
										sword->addComponent(physicsBody);
										_this->addChild(sword);

										_this->getScene()->getPhysicsWorld()->addJoint(PhysicsJointPin::construct(_this->getChildByName("alert"+_playerNumber)->getPhysicsBody(), physicsBody, _this->getChildByName("alert"+_playerNumber)->getPosition()+Vec2(sprite->getScaleX()*-50, -25.0f)));
									}),
									DelayTime::create(0.07f),
									CallFunc::create([&](){
										_this->getChildByTag(_pN*10000)->removeFromParent();
									}),
									DelayTime::create(0.6f),
									CallFunc::create([&](){
										_attackCD = false;
									}),
									nullptr);
					sprite->runAction(tmp);
				}
			} else {
				sprite->setVisible(false);
			}
			sprite->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
		}
	}

	void die() {
		_dead = true;
		if (!UserDefault::getInstance()->getBoolForKey("mute", false)) {
			AudioEngine::play2d("gameover.ogg", false);
		}
		
		UserDefault::getInstance()->setIntegerForKey("loser", _pN);
		UserDefault::getInstance()->flush();
		
		for (auto &sprite : _actions) {
			sprite->stopAllActions();
			if (sprite->getName() == "die"+_playerNumber) {
				sprite->setVisible(true);
				auto dieAnimation = Sequence::create(
										Spawn::createWithTwoActions(
											MusicFade::create(UserDefault::getInstance()->getIntegerForKey("backgroundMusic"), 1.5f, 0.0f, true),
											Animate::create(Animation::createWithSpriteFrames(_dieFrames, 1.0f/15))
										),
										CallFunc::create([&](){
											cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(_this);
											Director::getInstance()->replaceScene(Over::createScene());
										}),
										nullptr);
				sprite->runAction(dieAnimation);
				customFade(1.5f);
			} else if (sprite->getName() == "alert"+_playerNumber) {
				sprite->removeAllComponents();
				sprite->setVisible(false);
			} else {
				sprite->setVisible(false);
			}
		}
	}
	
	void customFade(float duration) {
		_this->runAction(FadeOut::create(duration));
		auto m = static_cast<cocos2d::TMXTiledMap*>(_this->getChildByName("map"));
		auto l = m->getLayer("Ground");
		for (int i=0; i<l->getLayerSize().width; i++) {
			for (int j=30; j<l->getLayerSize().height; j++) {
				l->getTileAt(Vec2(i, j))->runAction(FadeOut::create(duration));
			}
		}
	}

public:
	int damage;
	bool touchingGround = false;

	Player();
	Player(int pn, std::string c, Vec2 position, Scene* s) {
		_this = s;
		_pN = pn;
		_playerNumber = std::to_string(pn);
		_champion = c;

		if (_champion == "archer") {
			_health = 750;
			_speed = 300;
			_attackType = 1;
			damage = 100;
		} else if (_champion == "wizard") {
			_health = 500;
			_speed = 300;
			_attackType = 0;
			damage = 75;
		} else if (_champion == "knight") {
			_health = 1000;
			_speed = 200;
			_attackType = 2;
			damage = 200;
		}
		
		_healthRemaining = _health;

		auto physicsBody = PhysicsBody::createBox(Size(150, 300), PhysicsMaterial(0.0f, 0.0f, 1.0f));
		physicsBody->setRotationEnable(false);
		physicsBody->setLinearDamping(1.0f);
		physicsBody->setCategoryBitmask(0b00001);
		physicsBody->setCollisionBitmask(0b11010);
		physicsBody->setContactTestBitmask(0xFFFFFFFF);
		physicsBody->setGroup(-pn);

		auto alertFrames = getAnimation("Alert", 10);
		auto alert = Sprite::createWithSpriteFrame(alertFrames.front());
		alert->setName("alert"+_playerNumber);
		alert->setPosition(position);
		alert->setScale(0.5f);
		alert->addComponent(physicsBody);
		auto alertAnimation = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(alertFrames, 1.0f/10)));
		alert->runAction(alertAnimation);
		_this->addChild(alert);

		auto runFrames = getAnimation("Run", 10);
		auto run = Sprite::createWithSpriteFrame(runFrames.front());
		run->setName("run"+_playerNumber);
		run->setPosition(alert->getPosition());
		run->setScale(0.5f);
		auto runAnimation = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(runFrames, 1.0f/15)));
		run->runAction(runAnimation);
		_this->addChild(run);
		run->setVisible(false);

		_jumpFrames = getAnimation("Jump", 10);
		auto jump = Sprite::createWithSpriteFrame(_jumpFrames.front());
		jump->setName("jump"+_playerNumber);
		jump->setPosition(alert->getPosition());
		jump->setScale(0.5f);
		_this->addChild(jump);
		jump->setVisible(false);

		_attackFrames = getAnimation("Attack", 10);
		auto attack = Sprite::createWithSpriteFrame(_attackFrames.front());
		attack->setName("attack"+_playerNumber);
		attack->setPosition(alert->getPosition());
		attack->setScale(0.5f);
		_this->addChild(attack);
		attack->setVisible(false);

		_hitFrames = getAnimation("Hit", 10);
		auto hit = Sprite::createWithSpriteFrame(_hitFrames.front());
		hit->setName("hit"+_playerNumber);
		hit->setPosition(alert->getPosition());
		hit->setScale(0.5f);
		_this->addChild(hit);
		hit->setVisible(false);

		_dieFrames = getAnimation("Die", 10);
		auto die = Sprite::createWithSpriteFrame(_dieFrames.front());
		die->setName("die"+_playerNumber);
		die->setPosition(alert->getPosition());
		die->setScale(0.5f);
		_this->addChild(die);
		die->setVisible(false);
		
		_healthBase = Sprite::create("healthBase.png");
		_healthBase->setAnchorPoint(Vec2(0.5f, -6.0f));
		_healthBase->setPosition(alert->getPosition());
		_healthBase->setScale(0.2f);
		_this->addChild(_healthBase);
		_healthBar = LoadingBar::create("health"+std::to_string(_pN)+".png", 100.0f);
		_healthBar->setAnchorPoint(Vec2(0.5f, -6.0f));
		_healthBar->setPosition(alert->getPosition());
		_healthBar->setScale(0.2f);
		_this->addChild(_healthBar);

		_actions.push_back(alert);
		_actions.push_back(run);
		_actions.push_back(jump);
		_actions.push_back(attack);
		_actions.push_back(hit);
		_actions.push_back(die);

		if (pn == 2) {
			_healthBar->setScaleX(-0.2f);
			_healthBase->setScaleX(-0.2f);
			for (auto &sprite : _actions) {
				sprite->setScaleX(-0.5f);
			}
		}
	}

	void hit(int damage) {
		_hit = true;
		_healthRemaining -= damage;
		_healthBar->setPercent(std::max(0.0f, _healthRemaining*100.0f/_health));
		if (_healthRemaining <= 0) {
			die();
			return;
		}
		for (auto &sprite : _actions) {
			if (sprite->getName() == "hit"+_playerNumber) {
				sprite->setVisible(true);
				auto hitAnimation = Sequence::create(
										Animate::create(Animation::createWithSpriteFrames(_hitFrames, 1.0f/15)),
										CallFunc::create([&](){
											_hit = false;
										}),
										nullptr);
				sprite->runAction(hitAnimation);
			} else {
				sprite->setVisible(false);
			}
		}
	}

	void pressLeft() {_directions.push_back("left");}
	void stopLeft() {_directions.erase(remove(_directions.begin(), _directions.end(), "left"), _directions.end());}
	void pressRight() {_directions.push_back("right");}
	void stopRight() {_directions.erase(remove(_directions.begin(), _directions.end(), "right"), _directions.end());}
	void pressUp() {_singleAction.push_back("jump");}
	void stopUp() {_singleAction.erase(remove(_singleAction.begin(), _singleAction.end(), "jump"), _singleAction.end());}
	void pressAttack() {_singleAction.push_back("attack");}
	void stopAttack() {_singleAction.erase(remove(_singleAction.begin(), _singleAction.end(), "attack"), _singleAction.end());}

	void special() {
		if (!_dead && !_specialCD) {
			_specialCD = true;
			if (_champion == "wizard") {
				for (auto &sprite : _actions) {
					if (sprite->getName() == "alert"+_playerNumber) {
						sprite->setPositionX(std::max(std::min(sprite->getPositionX()+sprite->getScaleX()*500, Director::getInstance()->getVisibleSize().width), 0.0f));
						auto tmp = Sequence::create(
										DelayTime::create(1.0f),
										CallFunc::create([&](){
											_specialCD = false;
										}),
										nullptr);
						sprite->runAction(tmp);
					} else {
						sprite->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
					}
				}
				_healthBar->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
				_healthBase->setPosition(_healthBar->getPosition());
			} else if (_champion == "archer") {
				for (auto &sprite : _actions) {
					if (sprite->getName() == "alert"+_playerNumber) {
						auto tmp = Sequence::create(
										DelayTime::create(2.0f),
										CallFunc::create([&](){
											for (auto &sprite : _actions) {
												sprite->setOpacity(255);
											}
											_healthBar->setOpacity(255);
											_healthBase->setOpacity(255);
										}),
										DelayTime::create(2.0f),
										CallFunc::create([&](){
											_specialCD = false;
										}),
										nullptr);
						sprite->runAction(tmp);
					}
					sprite->setOpacity(0);
				}
				_healthBar->setOpacity(0);
				_healthBase->setOpacity(0);
			} else if (_champion == "knight") {
				for (auto &sprite : _actions) {
					if (sprite->getName() == "alert"+_playerNumber) {
						auto tmp = Sequence::create(
										DelayTime::create(0.2f),
										CallFunc::create([&](){
											_speed = 200;
										}),
										DelayTime::create(1.0f),
										CallFunc::create([&](){
											_specialCD = false;
										}),
										nullptr);
						sprite->runAction(tmp);
					}
					_speed = 1000;
				}
			}
		}
	}

	void update() {
		if (!_dead) {
			if (_directions.empty() && (_singleAction.empty() || !_jumpCD || !_attackCD) && touchingGround && !_attacking && !_hit) {
				reset();
			}

			if (!_directions.empty() && !_hit && !_attacking) {
				run(_directions.back());
			}

			if (!_singleAction.empty() && touchingGround && !_attacking && !_hit) {
				if (_singleAction.back() == "jump" && !_jumpCD) {
					jump();
				} else if (_singleAction.back() == "attack" && !_attackCD) {
					attack();
				}
			}

			for (auto &sprite : _actions) {
				sprite->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
			}
			_healthBar->setPosition(_this->getChildByName("alert"+_playerNumber)->getPosition());
			_healthBase->setPosition(_healthBar->getPosition());
		}
	}
};

#endif // __PLAYER_H__
