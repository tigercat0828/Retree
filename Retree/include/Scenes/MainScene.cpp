#include "pch.h"
#include "MainScene.h"
#include "Core/ResourceManager.h"
#include "Core/AssimpObject.h"
#include "Core/FileHelper.h"
#include "Core/RandomHelper.h"
#include "Core/Ray.h"
#include "Core/Shape2D.h"

float styleLeafSize = 0.08f;
string styleLeafModel = "multileaf/multileaf.obj";
string sytleLeafTexture = "branch2.jpg";
vector<TreeNode*> tTreeNodeCaches;
vec3 tSelectedNodePos = vec3(100,100,100);
TreeNode* tSelectedNode = nullptr;
bool doCamRevolve = false;

TreeNode* SelectTreeNode(const Ray& ray) {
	float minDist = std::numeric_limits<float>::max();
	TreeNode* closet = nullptr;
	for (TreeNode* node : tTreeNodeCaches) {
		vec3 v = node->position - ray.origin;
		float t = glm::dot(v, ray.direction);

		vec3 projPoint = ray.At(t);
		float dist = glm::distance(projPoint, node->position);

		if (dist < minDist) {
			closet = node;
			minDist = dist;
		}
	}
	if(minDist < 0.1f) return closet;
	return nullptr;
}
void UnselectTreeNode() {
	tSelectedNode = nullptr;
	tSelectedNodePos = vec3(100, 100, 100);
}

string Lstr = "F(0.4)[+(60)F(0.7)][-(60)F(0.7)]F(0.6)[+(60)F(0.49)][-(60)F(0.49)]F(0.5)[+(60)F(0.343)][-(60)F(0.343)]F(0.4)[+(60)F(0.24)][-(60)F(0.24)]F(0.3)";

void MainScene::OnLoad() {

	renderTarget = RenderTarget(800, 600);

	mPicker.AttachCamera(&Acam);
	mBoxSelector = BoxSelector(800, 600);

	Acam.LookAtFrom(vec3(0), vec3(8));
	
	entities.push_back(&Askybox);
	entities.push_back(&Agrid);

	Askybox.AllowDraw = true;
	Agrid.AllowDraw = false;

	Ttree = Tree::MakeTestTreeSmall();

	vec3 offset = Ttree->SetToLocalSpaceOrigin();
	tTreeNodeCaches = Ttree->GetAllTreeNodes();

	TtreeSkeletonRenderer.Attach(Ttree);


	//// TsubTree = Tree::ConstructFromLstring(Lstr,3, 0.1);
	TsubTree = Tree::ConstructFromLstring(Lstr, 0, 0);
	//cout << TsubTree->ToLsysGrammar() << endl;
	
	TsubTree->SetPosition(vec3(4, 0, 0));

	TsubTreeMeshRenderer.SetTreeStyle(sytleLeafTexture, styleLeafModel, styleLeafSize);
	TtreeMeshRenderer.SetTreeStyle(sytleLeafTexture, styleLeafModel, styleLeafSize);

	TsubTreeSkeletonRenderer.Attach(TsubTree);
	TsubTreeSkeletonRenderer.AllowDraw = true;


	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}


void MainScene::OnUpdate(float deltaTime) {

	float y = 1; 
	float d = 4;
	const vec3 focus(0, y, 0);
	if (Input.GetKeyDown(SDL_SCANCODE_1)) {
		Acam.LookAtFrom(focus, vec3(d ,y, 0));	// x axis
	}
	if (Input.GetKeyDown(SDL_SCANCODE_2)) {
		Acam.LookAtFrom(focus, vec3(0, y, d));	// z axis
	}
	if (Input.GetKeyDown(SDL_SCANCODE_3)) {
		Acam.LookAtFrom(focus, -vec3(d, y, 0));	// -x axis
	}
	if (Input.GetKeyDown(SDL_SCANCODE_4)) {
		Acam.LookAtFrom(focus, -vec3(0, y, d));	// -z axis
	}
	if (Input.GetKeyDown(SDL_SCANCODE_5)) {
		Acam.LookAtFrom(focus, vec3(0, d, 0));	// y axis
	}

	if (Input.GetKeyDown(SDL_SCANCODE_TAB)) {
		if (window.GetCursorMode() == CursorMode::CURSOR_LOCKED) {
			window.UnlockCursor();
			Acam.SetMotionMode(Camera::MotionMode::Orbit);
		}
		else {
			Acam.SetMotionMode(Camera::MotionMode::Free);
		}
	}

	if (Acam.GetMotionMode() == Camera::MotionMode::Orbit) {
		Acam.OnUpdate(deltaTime);
	}
	else {
		if (window.GetCursorMode() == CursorMode::CURSOR_LOCKED) Acam.OnUpdate(deltaTime);
	}
	
	if (doCamRevolve) Acam.Revolve(20.0f * deltaTime);

	// Select TreeNode
	if (Input.GetMouseButtonDown(SDL_BUTTON_LEFT)) {
		bool validRay = mPicker.CastRay(Input.GetMousePos() - cursorOffset, vec2(ImWindowWidth, ImWindowHeight));
		if (Ttree && validRay) {
			tSelectedNode = SelectTreeNode(mPicker.GetRay());
			if (tSelectedNode != nullptr) {
				tSelectedNodePos = tSelectedNode->position;
			}
		}

		mBoxSelector.SetActive(true);
		mBoxSelector.SetRectStart(Input.GetMousePos() - cursorOffset);
	}

	if (Input.GetMouseButtonPressed(SDL_BUTTON_LEFT)) {
		mBoxSelector.SetRectEnd(Input.GetMousePos() - cursorOffset);
	}
	if (Input.GetMouseButtonUp(SDL_BUTTON_LEFT)) {

		mBoxSelector.SetActive(false);
		mBoxSelector.GetRect();
	}
	mBoxSelector.Update();

	if (Input.GetKeyDown(SDL_SCANCODE_RIGHTBRACKET)) {		// print screen
		string file = renderTarget.SaveToFile();
		Logger::Info("frame save to file {}", file);
	}

	if (Input.GetKeyDown(SDL_SCANCODE_DELETE)) {
		TruncateTreeNode();
	}

	Input_ToggleViewing();
}

void MainScene::OnDraw() {

	renderTarget.Bind();	// draw to imgui texture
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, ImWindowWidth, ImWindowHeight);

	Abrush.Begin(Acam);
	if (mDoShowAxis) Abrush.DrawAxis();
	// highlight the select point
	Abrush.SetPointSize(8);
	Abrush.SetColor(Color::Red);
	Abrush.DrawPoint(tSelectedNodePos);
	

	Abrush.SetPointSize(1);
	Abrush.SetColor(Color::Yellow);
	Abrush.DrawPoint(Acam.GetFocusPoint());


	Abrush.SetColor(Color::Yellow);
	Abrush.End();


	for (auto& e : entities) e->OnDraw(Acam);

	
	TtreeSkeletonRenderer.OnDraw(Acam);
	TtreeMeshRenderer.OnDraw(Acam);
	TcloudRenderer.OnDraw(Acam);
	ToctreeRenderer.OnDraw(Acam);

	mBoxSelector.OnDraw();

	renderTarget.Unbind();
}

void MainScene::OnImGui() {
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
	ImWidget_MenuBar();
	ImWidget_SceneWindow();
	ImWidget_UtilsPanel();
	ImWidget_TreePanel();
}


void MainScene::ImWidget_UtilsPanel() {
	static std::vector<CamState> savedViews;
	CamState state = Acam.GetState();
	
	ImGui::Begin("Utils Panel");
	ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);

	auto mousePos = Input.GetMousePos();
	if (ImGui::Button("Orbit")) Acam.SetMotionMode(Camera::MotionMode::Orbit);
	ImGui::SameLine();
	if (ImGui::Button("Free")) {
		Acam.SetMotionMode(Camera::MotionMode::Free);
		window.LockCursor();
	} 

	ImGui::Text("Cursor Pos: %.f %.f", mousePos.x, mousePos.y);

	ImGui::Separator();

	// Edit current state
	if (ImGui::InputFloat3("Camera Pos", &state.position.x, "%.3f")) Acam.SetState(state);
	ImGui::Checkbox("Revolve", &doCamRevolve);
	if (ImGui::Button("Record View")) savedViews.push_back(state);
	ImGui::Separator();

	for (int i = 0; i < savedViews.size(); ++i) {
		ImGui::PushID(i);

		// Delete button
		if (ImGui::SmallButton("x")) {
			savedViews.erase(savedViews.begin() + i);
			ImGui::PopID();
			--i;
			continue;
		}
		ImGui::SameLine();

		// Go button
		if (ImGui::SmallButton("Go")) Acam.SetState(savedViews[i]);
		ImGui::SameLine();

		// Display
		const auto& s = savedViews[i];
		ImGui::Text("View #%d: Pos(%.1f, %.1f, %.1f)", i, s.position.x, s.position.y, s.position.z);
		ImGui::PopID();
	}

	ImGui::End();
}

void MainScene::ImWidget_MenuBar() {
	ImGui::BeginMainMenuBar();
	originAnchor = ImGui::GetWindowPos();
	if (ImGui::BeginMenu("Menu")) {
		if (ImGui::MenuItem("Exit", "Alt+F4"))  window.Close(); 
		ImGui::EndMenu();
	}
	Im_Menu_File();
	Im_Menu_Action();
	Im_Menu_View();
	ImGui::EndMainMenuBar();
}

void MainScene::ImWidget_SceneWindow() {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(540, 960), ImGuiCond_FirstUseEver);
	ImGui::Begin("Scene");

	auto anchor = ImGui::GetCursorScreenPos();
	cursorOffset = vec2(anchor.x - originAnchor.x, anchor.y - originAnchor.y);

	const float width = ImGui::GetContentRegionAvail().x;
	const float height = ImGui::GetContentRegionAvail().y;
	ImWindowWidth = width;
	ImWindowHeight = height;
	
	renderTarget.Rescale(width, height);
	Acam.SetAspectRatio(width, height);
	mBoxSelector.SetScreenSize(width, height);

	ImGui::GetWindowDrawList()->AddImage(
		(ImTextureID)(intptr_t)renderTarget.GetTextureID(),
		ImVec2(anchor.x, anchor.y),
		ImVec2(anchor.x + width, anchor.y + height),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
	ImGui::End();
	ImGui::PopStyleVar();
}


void MainScene::ImWidget_TreePanel() {
	static int trimDepth = 1;
	ImGui::Begin("tree styler");
	if (Ttree) {
		
		ImGui::InputFloat("leaf size", &styleLeafSize, 0.1f);
		if (ImGui::Button("single leaf")) {
			styleLeafModel = "greenleaf/greenleaf.obj";
			TsubTreeMeshRenderer.SetTreeStyle(sytleLeafTexture, styleLeafModel, styleLeafSize);
		}
		ImGui::SameLine();
		if (ImGui::Button("multi leaf")) {
			styleLeafModel = "multileaf/multileaf.obj";
			TsubTreeMeshRenderer.SetTreeStyle(sytleLeafTexture, styleLeafModel, styleLeafSize);
		}
		ImGui::SameLine();
		if (ImGui::Button("Generate Mesh")) {
			GenerateMesh();
		}

		ImGui::Text("Trim Tree"); 
		ImGui::SameLine();
		if (ImGui::Button("Leaf")) TrimTreeAllLeaves();
		ImGui::SameLine();
		if(ImGui::Button("Branch")) TrimTreeBranch();
		ImGui::SameLine();
		if (ImGui::Button("Twig")) TrimTreeLongTwigs();
		ImGui::Separator();

		ImGui::Text("  node count : %d", Ttree->GetNodeCount());
		ImGui::Text("branch count : %d", Ttree->GetBranchCount());
		ImGui::Text("  leaf count : %d", Ttree->GetLeafCount());
		ImGui::Text("   max depth : %d", Ttree->GetMaxDepth());
		ImGui::Text("   min depth : %d", Ttree->GetMinDepth());
		ImGui::Separator();
		if (tSelectedNode) {
			
			ImGui::Text("Selected Node : %d", tSelectedNode->ID);
			string parentStr;
			if (tSelectedNode->parent)
				parentStr = std::to_string(tSelectedNode->parent->ID);
			else 
				parentStr = "null";
			ImGui::Text("   parent : %s", parentStr);
			ImGui::Text("child num : %d", tSelectedNode->children.size());
			ImGui::Text("    depth : %d", tSelectedNode->depth);
			ImGui::Text("   radius : %f", tSelectedNode->radius);
			ImGui::Separator();
			if (ImGui::Button("Truncate")) TruncateTreeNode();
			ImGui::SameLine();
			if (ImGui::Button("Extract")) ExtractSubTree();
			ImGui::SameLine();
			if (ImGui::Button("Connect")) ConnectSubTree();
			ImGui::Separator();
			ImGui::Text("Create Twig");
			ImGui::SameLine();
			if (ImGui::Button("right")) CreateSmallTwig(tSelectedNode, Acam.GetPosition(), true); 
			ImGui::SameLine();
            if (ImGui::Button("left")) CreateSmallTwig(tSelectedNode, Acam.GetPosition(), false);
		}
		else {
			ImGui::Text("Selected Node : null");
		}
	}
	ImGui::End();
}

void MainScene::Im_SwapOrFlipCloud() {
	if (ImGui::MenuItem("Align Origin", "")) Tcloud->AlignToOrigin();
	ImGui::Separator();
	if (ImGui::MenuItem("Swap X-Y", "")) Tcloud->Swap(X_AXIS | Y_AXIS);
	if (ImGui::MenuItem("Swap X-Z", "")) Tcloud->Swap(X_AXIS | Z_AXIS);
	if (ImGui::MenuItem("Swap Y-Z   v", "")) Tcloud->Swap(Y_AXIS | Z_AXIS);
	ImGui::Separator();
	if (ImGui::MenuItem("Flip X", "")) Tcloud->Flip(X_AXIS);
	if (ImGui::MenuItem("Flip Y", "")) Tcloud->Flip(Y_AXIS);
	if (ImGui::MenuItem("Flip Z", "")) Tcloud->Flip(Z_AXIS);
}

void MainScene::Input_ToggleViewing() {

	if (Input.GetKeyDown(SDL_SCANCODE_P)) TcloudRenderer.AllowDraw = !TcloudRenderer.AllowDraw;
	if (Input.GetKeyDown(SDL_SCANCODE_N)) {
		TtreeSkeletonRenderer.AllowDraw = !TtreeSkeletonRenderer.AllowDraw;
		TsubTreeSkeletonRenderer.AllowDraw = !TsubTreeSkeletonRenderer.AllowDraw;
	}
	if (Input.GetKeyDown(SDL_SCANCODE_O)) ToctreeRenderer.AllowDraw = !ToctreeRenderer.AllowDraw;
	
	
	if (Input.GetKeyDown(SDL_SCANCODE_T)) {
		TtreeMeshRenderer.AllowDraw = !TtreeMeshRenderer.AllowDraw;
		TsubTreeMeshRenderer.AllowDraw = !TsubTreeMeshRenderer.AllowDraw;
	}
	if (Input.GetKeyDown(SDL_SCANCODE_E)) {
		TtreeMeshRenderer.EnableWindEffect = !TtreeMeshRenderer.EnableWindEffect;
		TsubTreeMeshRenderer.EnableWindEffect = !TsubTreeMeshRenderer.EnableWindEffect;
	}
	if (Input.GetKeyDown(SDL_SCANCODE_B)) {
		TtreeMeshRenderer.AllowDrawBranch = !TtreeMeshRenderer.AllowDrawBranch;
		TsubTreeMeshRenderer.AllowDrawBranch = !TsubTreeMeshRenderer.AllowDrawBranch;

	}
	if (Input.GetKeyDown(SDL_SCANCODE_L)) {
		TtreeMeshRenderer.AllowDrawLeave = !TtreeMeshRenderer.AllowDrawLeave;
		TsubTreeMeshRenderer.AllowDrawLeave = !TsubTreeMeshRenderer.AllowDrawLeave;
	}

	if (Input.GetKeyDown(SDL_SCANCODE_K)) Askybox.AllowDraw = !Askybox.AllowDraw;
	if (Input.GetKeyDown(SDL_SCANCODE_G)) Agrid.AllowDraw = !Agrid.AllowDraw;
	if (Input.GetKeyDown(SDL_SCANCODE_X)) mDoShowAxis = !mDoShowAxis;
	if (Input.GetKeyDown(SDL_SCANCODE_Q)) UnselectTreeNode();

	if (Input.GetKeyDown(SDL_SCANCODE_R)) doCamRevolve = !doCamRevolve;

}

void MainScene::Im_Menu_File() {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Open", "")) {
			string filename = FileHelper::Open("ply,xyz");
			if (filename != "") {
				if (Tcloud) {
					delete Tcloud;
					Tcloud = nullptr;
				}
				if (Ttree) {
					delete Ttree;
					Ttree = nullptr;
				}
				ToctreeRenderer.AllowDraw = false;
				TtreeSkeletonRenderer.AllowDraw = false;
                TtreeMeshRenderer.AllowDraw = false;

				Logger::Info("Open {}", filename);
				Tcloud = new PointCloud(filename);
				Tcloud->SetPointColorGradient(Color::Orange, Color::Orange);
				Tcloud->AlignToOrigin();
				Acam.FitBoxtToScreen(Tcloud->GetBoundingBox());
				TcloudRenderer.Attach(*Tcloud);
				TcloudRenderer.AllowDraw = true;
			}
		}
		if (ImGui::BeginMenu("Save")) {
			if (ImGui::MenuItem("Point Cloud")) {
				string filename = FileHelper::Save("xyz");
				Tcloud->SaveXYZ(filename + ".xyz");
				Logger::Info("Save file \"{}\"", filename);
			}
			if (ImGui::MenuItem("Tree Model")) {
				string filename = FileHelper::Save("obj");
				// todo
				Logger::Info("Save file \"{}\"", filename);
			}
			ImGui::EndMenu();
		}
	
		ImGui::EndMenu();
	}
}

void MainScene::Im_Menu_Action() {

	if (ImGui::BeginMenu("Action")) {
		
		ImGui::Separator();
		if (ImGui::MenuItem("Perform Pipeline", "")) {
			if (Tcloud == nullptr) Logger::Warn("Need to load point cloud file first");
			else {
				GenerateSkeleton();
				ConstructOctree();
				GenerateMesh();
				TcloudRenderer.AllowDraw = false;
				TtreeSkeletonRenderer.AllowDraw = false;
				TtreeMeshRenderer.AllowDraw = true;
			}
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Generate Skeleton", "")) {
			if (Tcloud == nullptr) Logger::Warn("Need to load point cloud file first");
			else {
				GenerateSkeleton();
				//ConstructOctree();
				TcloudRenderer.AllowDraw = false;
				TtreeSkeletonRenderer.AllowDraw = true;
			}
		}
		if (ImGui::MenuItem("Generate Mesh", "")) {
			if(Ttree == nullptr) Logger::Warn("Need to generate skelton first");
			else {
				GenerateMesh();
				TtreeMeshRenderer.AllowDraw = true;
			}
		}
		ImGui::Separator();
		
		if (ImGui::MenuItem("Trim", "")) {
			Ttree->TrimAllLeaves();
			TtreeSkeletonRenderer.Attach(Ttree);
		}
	
		ImGui::Separator();

		Im_SwapOrFlipCloud();

		ImGui::EndMenu();
	}
}

void MainScene::Im_Menu_View() {
	if (ImGui::BeginMenu("View")) {
		ImGui::Checkbox("PointCloud        (P)", &TcloudRenderer.AllowDraw);
		ImGui::Checkbox("Skeleton          (N)", &TtreeSkeletonRenderer.AllowDraw);
		ImGui::Checkbox("Octree            (O)", &ToctreeRenderer.AllowDraw);
		ImGui::Separator();
		ImGui::Checkbox("TreeMesh          (T)", &TtreeMeshRenderer.AllowDraw);
		ImGui::Checkbox("Branch            (B)", &TtreeMeshRenderer.AllowDrawBranch);
        ImGui::Checkbox("Leaf              (L)", &TtreeMeshRenderer.AllowDrawLeave);
		ImGui::Checkbox("Wind Effect       (E)", &TtreeMeshRenderer.EnableWindEffect);
		ImGui::Separator();
		ImGui::Checkbox("Grid              (G)", &Agrid.AllowDraw);
		ImGui::Checkbox("Skybox            (K)", &Askybox.AllowDraw);
		ImGui::Checkbox("Axis              (X)", &mDoShowAxis);
		ImGui::Checkbox("Revolve           (R)", &doCamRevolve);
		ImGui::Separator();
		if (ImGui::Button("Black Background")) glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		ImGui::SameLine();
		if (ImGui::Button("White Background")) glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::EndMenu();

	}
}


vector<vec2> GenerateRandomPositions(int count, vec3 center, float radius, float minDist = 2.0f, int maxTries = 100) {
	std::vector<vec2> result;
	for (int i = 0; i < count; ++i) {
		vec2 newPos;
		bool valid = false;
		for (int attempt = 0; attempt < maxTries; ++attempt) {
			newPos = RandomHelper::RandomInCircle(radius, center);
			valid = true;
			for (const vec2& existing : result) {
				if (glm::distance(existing, newPos) < minDist) {
					valid = false;
					break;
				}
			}
			if (valid) break;
		}
		if (valid) {
			result.push_back(newPos);
		}
		else {
			Logger::Warn("Failed to generate valid position for index {}", i);
		}
	}
	return result;
}

void MainScene::GenerateMesh() {
	TtreeMeshRenderer.SetTreeStyle(sytleLeafTexture, styleLeafModel , styleLeafSize);
	TtreeMeshRenderer.Attach(Ttree);

}
void MainScene::GenerateSkeleton() {

	string xzyFile;
	xzyFile = Tcloud->SaveXZY("adinput.xyz");
	Adtree::Run(xzyFile);	// generate ADText.txt
	PosGraph graph = PosGraph("ADtext.txt");

	Tcloud->SetPointColorGradient(Color::Orange, Color::Orange);
	TcloudRenderer.AllowDraw = false;

	Ttree = Tree::ConstructFromGraph(graph);

	vec3 offset = Ttree->SetToLocalSpaceOrigin();
	tTreeNodeCaches = Ttree->GetAllTreeNodes();
	Tcloud->Translate(-offset);
	TtreeSkeletonRenderer.Attach(Ttree);
	TtreeSkeletonRenderer.AllowDraw = true;
	TcloudRenderer.AllowDraw = false;
}

void MainScene::ConstructOctree() {
	
	Box3D box = Tcloud->GetBoundingBox(); box = Box3D(box.min - vec3(0.1f), box.max + vec3(0.1f));
	Rect3D boundary(box.GetRect());
	Toctree = Octree<TreeNode*>(boundary);
	
	for (const auto& p : Ttree->GetAllTreeNodes()) Toctree.Insert(p->position, p);
	ToctreeRenderer.Attach(Toctree);
	ToctreeRenderer.AllowDraw = true;
	Logger::Debug("construct octree done");
}

void MainScene::TruncateTreeNode() {
	Ttree->Truncate(tSelectedNode);

	TtreeSkeletonRenderer.MarkDirty();
	// and refresh the node caches
	tTreeNodeCaches = Ttree->GetAllTreeNodes();
	GenerateMesh();
}

void MainScene::ExtractSubTree() {
	if (TsubTree) {
		delete TsubTree;
		TsubTree = nullptr;
	}
	TsubTree = Ttree->ExtractSubTree(tSelectedNode);

	float y = TsubTree->GetRoot()->position.y;
	
	TsubTree->SetToLocalSpaceOrigin();
	TsubTree->SetPosition(vec3(10, 0, 0));

	TsubTreeSkeletonRenderer.Attach(TsubTree);
	TsubTreeSkeletonRenderer.AllowDraw = true;

	TsubTreeMeshRenderer.SetTreeStyle(sytleLeafTexture, styleLeafModel, styleLeafSize);
	TsubTreeMeshRenderer.EnableWindEffect = false;
	TsubTreeMeshRenderer.Attach(TsubTree);
}

void MainScene::TrimTreeAllLeaves() {
	Ttree->TrimAllLeaves();
	TtreeSkeletonRenderer.MarkDirty();
	TtreeMeshRenderer.MarkDirty();
	tTreeNodeCaches = Ttree->GetAllTreeNodes();
}

void MainScene::TrimTreeLongTwigs() {
	Ttree->TrimLongTwigs();
	TtreeSkeletonRenderer.MarkDirty();
	TtreeMeshRenderer.MarkDirty();
	tTreeNodeCaches = Ttree->GetAllTreeNodes();
}

void MainScene::TrimTreeBranch() {
	Ttree->TrimBranch();
	TtreeSkeletonRenderer.MarkDirty();
	TtreeMeshRenderer.MarkDirty();
	tTreeNodeCaches = Ttree->GetAllTreeNodes();
}

void MainScene::ConnectSubTree() {
	Ttree->Connect(tSelectedNode, TsubTree);
	TtreeSkeletonRenderer.MarkDirty();
	TtreeMeshRenderer.MarkDirty();
	tTreeNodeCaches = Ttree->GetAllTreeNodes();
}

void MainScene::CreateSmallTwig(TreeNode* tnode, const vec3& camPos, bool right) {

	if (tSelectedNode) {
		Logger::Debug("ID {}  cam{}", tnode->ID, glm::to_string(camPos));
		Ttree->CreateSmallTwigForConnection(tnode, camPos, right);
		TtreeSkeletonRenderer.MarkDirty();
		TtreeMeshRenderer.MarkDirty();
		tTreeNodeCaches = Ttree->GetAllTreeNodes();
	}
}

void MainScene::OnExit() {
	if (Ttree) delete Ttree;
	if (Tcloud) delete Tcloud;
}

/*
Tree sytler
ImGui::InputFloat("leaf size##1", &gLeafSize);

	if (ImGui::Button("Single Leaf")) gLeafModel = "greenleaf/greenleaf.obj";
	if (ImGui::Button("Multiple Leaf")) gLeafModel = "multileaf/multileaf.obj";

	ImGui::Separator();
	if (ImGui::Button("Branch 1")) gLeafTexture = "branch.jpg";
	if (ImGui::Button("Branch 2")) gLeafTexture = "branch2.jpg";

	ImGui::Separator();
	if (ImGui::Button("Trim")) {
		Ttree->Trim(1);
		Ttree->CalcTreeInfo();
		TtreeSkeletonRenderer.Attach(Ttree);
	}
	if (ImGui::Button("Change Style")) GenerateMesh();
	if (ImGui::Button("octree")) ConstructOctree();

*/