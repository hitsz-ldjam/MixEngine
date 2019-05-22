#include"SceneManager.h"

namespace Mix
{
	size_t SceneManager::SceneCount()
	{
		return mScene.size();
	}
	Scene* SceneManager::GetActiveScene()//
	{
		return mIsActive;
	}
	Scene* SceneManager::CreateScene(const std::string &str)
	{
		Scene *ret = new Scene(str);
		mScene.insert(ret);
		return ret;
	}
	void SceneManager::LoadScene(size_t _sceneBuildIndex)
	{
		std::set<Scene*>::iterator iter = mScene.begin();
		while ((*iter)->buildIndex() != _sceneBuildIndex)
		{
			iter++;
		}
		(*iter)->getLoaded();
	}
	void SceneManager::LoadScene(const std::string& _scenename)
	{
		std::set<Scene*>::iterator iter;
		iter = mScene.begin();
		while ((*iter)->myName() != _scenename)
		{
			iter++;
		}
		(*iter)->getLoaded();
	}
	bool SceneManager::MakeSceneActive(Scene *s)
	{
		return (mIsActive = s);
	}
}