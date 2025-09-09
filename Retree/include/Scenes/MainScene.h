#pragma once
#include "Core/RenderTarget.h"
#include "Core/Scene.h"
#include "Core/Window.h"
#include "Core/InputManager.h"
#include "Core/Primitives.h"
#include "Core/Brush3D.h"
#include "Core/GameObject.h"
#include "Core/Octree.h"
#include "Prefabs/Skybox.h"
#include "Prefabs/EndlessGrid.h"
#include "Retree/Headers.h"
#include "Core/OctreeRenderer.h"
#include <memory>
#include <Core/MousePicker.h>
#include <Core/BoxSelector.h>

using namespace glm;
using namespace std;

class MainScene : public Scene {
private:	// global
	InputManager& Input = InputManager::Instance();
	Window& window = Window::Instance();
private:	// for imgui scene view window
	ImVec2 originAnchor;
	vec2 cursorOffset;
	int ImWindowWidth{ 800 };
	int ImWindowHeight{ 600 };
	RenderTarget renderTarget;
private:
	bool mDoShowAxis = false;
private: // Assets
	Camera Acam;
	Skybox Askybox;
	EndlessGrid Agrid;
	Brush3D Abrush;
	MousePicker mPicker;
	BoxSelector mBoxSelector;

	//target
	PointCloud* Tcloud = nullptr;
	Tree* Ttree = nullptr;
	Octree<TreeNode*> Toctree;
	Tree* TsubTree = nullptr;

	OctreeRenderer<TreeNode*> ToctreeRenderer;
	TreeMeshRenderer TtreeMeshRenderer;
	TreeSkeletonRenderer TtreeSkeletonRenderer;
	PointCloudRenderer TcloudRenderer;
	
	TreeMeshRenderer TsubTreeMeshRenderer;
	TreeSkeletonRenderer TsubTreeSkeletonRenderer;
	
	
	vector<GameObject*> entities;
	
public:
	MainScene() = default;
	void OnLoad() override;
	void OnUpdate(float deltaTime) override;
	void OnDraw() override;
	void OnEvent(const SDL_Event& event) override {}
	void OnExit() override;
	void OnImGui() override;
	void ImWidget_MenuBar();
	void ImWidget_UtilsPanel();
	void ImWidget_SceneWindow();
	void ImWidget_TreePanel();
	
	void Im_Menu_File();
	void Im_Menu_Action();
	void Im_SwapOrFlipCloud();
	void Im_Menu_View();
	void Input_ToggleViewing();
private:
	void GenerateSkeleton();
	void GenerateMesh();
	void ConstructOctree();


	void TruncateTreeNode();
	void ExtractSubTree();
	void TrimTreeAllLeaves();
	void TrimTreeLongTwigs();
	void TrimTreeBranch();
	void ConnectSubTree();
	void CreateSmallTwig(TreeNode* tnode, const vec3& camPos, bool right);

	// the direction according the next trunk node's branch direction
	// so we have to find the next trunk node and it is branch node too (child >=2) 
	// the direction should be 180 degree counter;
};

