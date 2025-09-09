#pragma once
#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>

using std::shared_ptr;
using std::function;

template <typename Signature = void()>
class IObservable {
public:
    using Observer = shared_ptr<function<Signature>>;

    void AddObserver(const Observer& observer) {
        mObservers.push_back(observer);
    }

    void RemoveObserver(const Observer& observer) {
        mObservers.erase(
            std::remove(mObservers.begin(), mObservers.end(), observer),
            mObservers.end()
        );
    }

protected:
    void NotifyObservers() {
        for (const auto& cb : mObservers) {
            if (cb && *cb) (*cb)();
        }
    }

private:
    vector<Observer> mObservers;
};
