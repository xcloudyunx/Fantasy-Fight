#include "MusicFade.h"
#include "AudioEngine.h"

using namespace cocos2d;
using namespace experimental;

MusicFade::MusicFade()
{
    m_initialVal = 0;
    m_targetVal = 0;
}

MusicFade* MusicFade::create(int target, float duration, float volume, bool stopOnComplete)
{
    MusicFade *pAction = new MusicFade();
    pAction->initWithDuration(target, duration, volume, stopOnComplete);
    pAction->autorelease();

    return pAction;
}

bool MusicFade::initWithDuration(int target, float duration, float volume, bool stopOnComplete)
{
    if (ActionInterval::initWithDuration(duration))
    {
		m_audioID = target;
        m_targetVal = volume;
        m_stopOnComplete = stopOnComplete;
        return true;
    }

    return false;
}

void MusicFade::update(float time)
{
    float vol = m_initialVal + time*(m_targetVal - m_initialVal);
    AudioEngine::setVolume(m_audioID, vol);

}

void MusicFade::startWithTarget(Node *pTarget)
{
    ActionInterval::startWithTarget( pTarget );
    m_initialVal = AudioEngine::getVolume(m_audioID);
}

void MusicFade::stop(void)
{
    if(m_stopOnComplete) AudioEngine::stop(m_audioID);
    ActionInterval::stop();
}