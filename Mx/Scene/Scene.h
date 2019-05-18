//#include"Mx/MxGameObject.h"
#include<cstring>
#include<vector>
#include<iterator>
#include<algorithm>
#include<iostream>
#define GameObject int

namespace Mix {
	class Scene {
	public:
		Scene(const std::string &str);
		~Scene();

		void addGameObject(GameObject* _obj);
		void removeGameObject(GameObject* _obj);
		void getLoaded();

		std::vector<GameObject*> getRootGameObject();

		bool isLoad();
		size_t buildIndex() const;
		std::string myName() const;
	private:
		std::vector<GameObject*> mHierarchy;
		size_t mBuildIndex;
		bool mIsLoaded;
		std::string mName;
	};

	
}