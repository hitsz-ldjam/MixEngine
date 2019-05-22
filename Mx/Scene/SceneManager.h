#include"Mx/MxGameObject.h"
#include"Scene.h"

#include<set>
#include<cstring>
#include<iterator>
#include<algorithm>
#include<iostream>


namespace Mix {
	class SceneManager {
	public:
		size_t SceneCount();
		Scene* CreateScene(const std::string &str);
		Scene* GetActiveScene();
		bool MakeSceneActive(Scene *s);
		void LoadScene(size_t _sceneBuildIndex);
		void LoadScene(const std::string& _scenename);
	private:
		std::set<Scene*> mScene;
		Scene *mIsActive;
	};
}
