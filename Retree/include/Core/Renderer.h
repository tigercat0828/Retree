#pragma once
#include <functional>
#include <memory>

using std::function;
using std::shared_ptr;

class Camera;
class Renderer {
public:
	virtual ~Renderer() = default;
	virtual void OnDraw(Camera& cam) = 0;
	void MarkDirty() { mDirtyFlag = true; }
	bool AllowDraw = true;
protected:
	bool mDirtyFlag{ false };
	shared_ptr<function<void()>> mDirtyCallback;
	virtual void SetupGL() = 0;
	virtual void UpdateBuffer() = 0;
	virtual void UploadBuffer() = 0;
public:
	virtual void ReleaseGL() = 0;
};