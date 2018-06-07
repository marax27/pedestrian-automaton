#ifndef _PEDESTRIAN_PRIORITY_QUEUE_H_
#define _PEDESTRIAN_PRIORITY_QUEUE_H_

#include "pedestrian.h"
#include "field.h"
#include <queue>
#include <map>

namespace sim{

// A priority queue which sorts pedestrians according 
// to values of field called key_provider.

template<typename T>
class PedestrianPriorityQueue{
	struct Node;
	struct NodeComparator;

	typedef std::priority_queue<Node, std::vector<Node>, NodeComparator> internal_queue;
public:
	PedestrianPriorityQueue(const Field<T> &_key_provider,
	                        const std::map<uid_t, Pedestrian> &_pedestrians)
		: key_provider(_key_provider), pedestrians(_pedestrians) {}

	inline void push(uid_t id){
		auto pos = pedestrians.at(id).getPosition();
		pqueue.push(Node{id, key_provider.at(pos.x, pos.y)});
	}

	inline uid_t top(){
		return pqueue.top().id;
	}

	inline void pop(){
		pqueue.pop();
	}

	inline bool empty() const {
		return pqueue.empty();
	}

	inline index_t size() const {
		return pqueue.size();
	}

private:
	const Field<T> &key_provider;
	const std::map<uid_t, Pedestrian> &pedestrians;

	internal_queue pqueue;

	struct Node{
		uid_t id;
		T key;
	};
	struct NodeComparator{
		bool operator()(const Node &a, const Node &b){
			return a.key < b.key;
		}
	};
};

}

#endif  //_PEDESTRIAN_PRIORITY_QUEUE_H_