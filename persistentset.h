#ifndef PERSISTENTSET_H_
#define PERSISTENTSET_H_

#include <memory>

struct persistent_set {
	typedef int value_type;

	struct node;
	struct root_node;

	struct iterator;

	persistent_set();

	persistent_set(persistent_set const& other);

	persistent_set& operator=(persistent_set const& rhs);

	~persistent_set() = default;

	iterator find(value_type);

	std::pair<iterator, bool> insert(value_type);

	void erase(iterator);

	iterator begin() const;
	iterator end() const;

	std::shared_ptr<persistent_set::root_node> root;

	static std::shared_ptr<const persistent_set::node> endnode;

	void printset() const;
};

struct persistent_set::node: std::enable_shared_from_this<persistent_set::node> {
	const value_type data;
	std::shared_ptr<const persistent_set::node> left, right;
	std::shared_ptr<const persistent_set::node> self() const;

	node() = delete;
	node(value_type data, std::shared_ptr<const persistent_set::node> left,
			std::shared_ptr<const persistent_set::node> right);

	std::shared_ptr<const persistent_set::node> find_key(
			const value_type & key) const;
	std::shared_ptr<const persistent_set::node> find_prev(
			const value_type & key) const;
	std::shared_ptr<const persistent_set::node> find_next(
			const value_type & key) const;

	std::shared_ptr<const persistent_set::node> find_first() const;
	std::shared_ptr<const persistent_set::node> find_last() const;

	std::pair<std::shared_ptr<const persistent_set::node>,
			std::shared_ptr<const persistent_set::node>> insert(
			const value_type & key) const;

	std::shared_ptr<const persistent_set::node> erase(
			const value_type & key) const;

	void print_tree() const;
};

struct persistent_set::root_node {
	std::shared_ptr<const persistent_set::node> root;
	root_node() = delete;
	root_node(std::shared_ptr<const persistent_set::node> root);
};

struct persistent_set::iterator {
	std::weak_ptr<persistent_set::root_node> root;
	std::weak_ptr<const persistent_set::node> data;

	iterator(std::weak_ptr<persistent_set::root_node> root,
			std::weak_ptr<const persistent_set::node> data);

	value_type const& operator*() const;

	iterator& operator++();
	iterator operator++(int);

	iterator& operator--();
	iterator operator--(int);
};

#endif /* PERSISTENTSET_H_ */
