#include"Scene.h"


namespace Mix {
	 
	 Scene::Scene(const std::string &str)
	 {
		 mName = str;
		 mIsLoaded = false;
		 mBuildIndex = 0;
	 }
	 std::string Scene::myName() const
	 {
		 return mName;
	 }
	 
	 void Scene::addGameObject(GameObject* _obj)
	 {
		 mHierarchy.push_back(_obj);
	 }
	 void Scene::removeGameObject(GameObject* _obj)
	 {
		 std::vector<GameObject*>::iterator iter = std::find(mHierarchy.begin(), mHierarchy.end(), _obj);
		 mHierarchy.erase(iter);
	 }
	 void Scene::getLoaded()
	 {
		 mIsLoaded = true;
	 }
	 bool Scene::isLoad()
	 {
		 return mIsLoaded;
	 }
	

	 std::vector<GameObject*> Scene::getRootGameObject()
	 {
		 return mHierarchy;
	 }
	 size_t Scene::buildIndex() const
	 {
		 return mBuildIndex;
	 }

}