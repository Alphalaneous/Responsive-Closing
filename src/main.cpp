#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace geode::prelude;

LONG_PTR oWindowProc;
bool isReloading = false;

HWND getWindowHandle() {
    return WindowFromDC(wglGetCurrentDC());
}

class DummyAudio {
	public:
	void stopAudio(float dt) {
		FMODAudioEngine::get()->stopAllMusic(true);
	}
};

void closing() {
	if (!isReloading) {
		ShowWindow(getWindowHandle(), 0);
		FMODAudioEngine::get()->scheduleOnce(schedule_selector(DummyAudio::stopAudio), 0.5f);
		FMODAudioEngine::get()->fadeOutMusic(0.5f, 0);
		FMODAudioEngine::get()->stopAllEffects();
	}
}

LRESULT CALLBACK nWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_DESTROY:
    case WM_QUIT:
    case WM_CLOSE:
		closing();
        break;
    }

    return CallWindowProc((WNDPROC)oWindowProc, hwnd, msg, wparam, lparam);
}

void modifyWindowProc() {
	oWindowProc = SetWindowLongPtrA(getWindowHandle(), -4, (LONG_PTR)nWindowProc);
}

$on_mod(Loaded) {
	modifyWindowProc();
}

class $modify(MyGameManager, GameManager) {

    void reloadAll(bool switchingModes, bool toFullscreen, bool borderless, bool fix, bool unused) {
		isReloading = true;
    	GameManager::reloadAll(switchingModes, toFullscreen, borderless, fix, unused);
	}


	void reloadAllStep5() {
		GameManager::reloadAllStep5();
		modifyWindowProc();
		isReloading = false;
	}
};

class $modify(MyMenuLayer, MenuLayer) {

    void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
		
		if (p0->getTag() == 0 && p1) {
			closing();
		}
		MenuLayer::FLAlert_Clicked(p0, p1);
	}

};
