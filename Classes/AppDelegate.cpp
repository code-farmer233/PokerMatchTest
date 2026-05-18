#include "AppDelegate.h"
#include "PokerScene.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview)
    {
        glview = GLViewImpl::createWithRect("Test", cocos2d::Rect(0, 0, 1080, 2080), 0.5);
        director->setOpenGLView(glview);
    }

    glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::FIXED_WIDTH);
    director->setAnimationInterval(1.0f / 60.0f);

    director->runWithScene(PokerScene::createScene());
    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
}
