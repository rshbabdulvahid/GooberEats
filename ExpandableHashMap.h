#ifndef EXPENDABLEHASHMAP_H
#define EXPENDABLEHASHMAP_H

#include <list>

// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	struct Bucket {
		KeyType key;
		ValueType value;
	};
	std::list<Bucket>* m_buckets;
	double maxLoad;
	int bucketCount;
	int numElements;
	unsigned int getBucketNumber(const KeyType& key) const;
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
	if (maximumLoadFactor < 0)
		maxLoad = 0.5;
	else
		maxLoad = maximumLoadFactor;
	m_buckets = new std::list<Bucket>[8];
	numElements = 0;
	bucketCount = 8;
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
	delete[] m_buckets;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	delete[] m_buckets;
	numElements = 0;
	m_buckets = new std::list<Bucket>[8];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	return numElements;
}

template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key) const 
{
	unsigned int hasher(const KeyType& k); // prototype
	unsigned int h = hasher(key);
	return h % bucketCount;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	if (find(key) == nullptr) {
		if (((double)(numElements + 1) / (double)bucketCount) > maxLoad) {
			std::list<Bucket>* temp = new std::list<Bucket>[2 * bucketCount];
			int prevSize = bucketCount;
			bucketCount *= 2;
			typename std::list<Bucket>::iterator it;
			for (int i = 0; i < prevSize; i++) {
				if (m_buckets[i].empty() == false) {
					for (it = m_buckets[i].begin(); it != m_buckets[i].end(); it++) {
						Bucket myBuck;
						myBuck.key = (*it).key;
						myBuck.value = (*it).value;
						int index = getBucketNumber(myBuck.key);
						temp[index].push_back(myBuck);
					}
				}
			}
			delete[] m_buckets;
			m_buckets = temp;
			int index = getBucketNumber(key);
			Bucket myBuck;
			myBuck.key = key;
			myBuck.value = value;
			m_buckets[index].push_back(myBuck);
			numElements++;
		}
		else {
			int index = getBucketNumber(key);
			Bucket myBuck;
			myBuck.key = key;
			myBuck.value = value;
			m_buckets[index].push_back(myBuck);
			numElements++;
		}
	}
	else {
		*(find(key)) = value;
	}
}


template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	int index = getBucketNumber(key);
	if (m_buckets[index].empty() == false) {
		typename std::list<Bucket>::iterator it;
		for (it = m_buckets[index].begin(); it != m_buckets[index].end(); it++) {
			if (it->key == key)
				return &(it->value);
		}
		return nullptr;
	}
	return nullptr;
}

#endif