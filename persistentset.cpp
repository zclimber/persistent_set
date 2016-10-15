#include <persistentset.h>

#include <cassert>

#include <cstdio>

std::shared_ptr<const persistent_set::node> persistent_set::endnode = std::make_shared<const persistent_set::node>(value_type(), nullptr, nullptr);

persistent_set::persistent_set() {
	root = std::make_shared<root_node>(nullptr);
}

persistent_set::persistent_set(const persistent_set & other) :
		root(other.root) {
}

persistent_set& persistent_set::operator=(persistent_set const& rhs) {
	this->root = rhs.root;
	return *this;
}

persistent_set::iterator persistent_set::find(value_type key) {
	auto iter = root->root->find_key(key);
	return iterator(root, iter);
}

std::pair<persistent_set::iterator, bool> persistent_set::insert(
		value_type key) {
	auto iter = root->root->find_key(key);
	if (iter != endnode) {
		return {iterator(root, iter), false};
	} else {
		auto ins = root->root->insert(key);
		root = std::make_shared<persistent_set::root_node>(ins.first);
		return {persistent_set::iterator(root, ins.second), true};
	}
}

void persistent_set::erase(persistent_set::iterator target) {
	assert(target.root.expired() == false);
	assert(target.root.lock() == root);
	assert(target.data.expired() == false);
	assert(target.data.lock() != endnode);
	auto nroot = root->root->erase(target.data.lock()->data);
	root = std::make_shared<persistent_set::root_node>(nroot);
}

persistent_set::iterator persistent_set::begin() const {
	auto iter = root->root->find_first();
	return iterator(root, iter);
}

persistent_set::iterator persistent_set::end() const {
	return iterator(root, endnode);
}

std::shared_ptr<const persistent_set::node> persistent_set::node::self() const {
	return shared_from_this();
}

persistent_set::node::node(value_type data, std::shared_ptr<const node> left,
		std::shared_ptr<const node> right) :
		data(data), left(left), right(right) {
}

std::shared_ptr<const persistent_set::node> persistent_set::node::find_key(
		const value_type & key) const {
	if (this == nullptr) {
		return endnode;
	}
	if (this->data == key) {
		return this->self();
	}
	if (key < this->data) {
		return this->left->find_key(key);
	} else {
		return this->right->find_key(key);
	}
}

std::shared_ptr<const persistent_set::node> persistent_set::node::find_prev(
		const value_type & key) const {
	if (this == nullptr) {
		return endnode;
	}
	if (key <= this->data) {
		return this->left->find_prev(key);
	} else {
		auto res = this->right->find_prev(key);
		if (res == endnode) {
			return this->self();
		} else {
			return res;
		}
	}
}

std::shared_ptr<const persistent_set::node> persistent_set::node::find_next(
		const value_type & key) const {
	if (this == nullptr) {
		return endnode;
	}
	if (key >= this->data) {
		return this->right->find_next(key);
	} else {
		auto res = this->left->find_next(key);
		if (res == endnode) {
			return this->self();
		} else {
			return res;
		}
	}
}

std::shared_ptr<const persistent_set::node> persistent_set::node::find_first() const {
	if (this == nullptr) {
		return endnode;
	}
	if (this->left == nullptr) {
		return this->self();
	} else {
		return this->left->find_first();
	}
}
std::shared_ptr<const persistent_set::node> persistent_set::node::find_last() const {
	if (this == nullptr) {
		return endnode;
	}
	if (this->right == nullptr) {
		return this->self();
	} else {
		return this->right->find_last();
	}
}

std::pair<std::shared_ptr<const persistent_set::node>,
		std::shared_ptr<const persistent_set::node>> persistent_set::node::insert(
		const value_type & key) const {
	if (this == nullptr) {
		auto res = std::make_shared<persistent_set::node>(key, nullptr,
				nullptr);
		return {res, res};
	}
	if (key < this->data) {
		auto res = this->left->insert(key);
		auto new_node = std::make_shared<persistent_set::node>(this->data,
				res.first, this->right);
		return {new_node, res.second};
	} else {
		auto res = this->right->insert(key);
		auto new_node = std::make_shared<persistent_set::node>(this->data,
				this->left, res.first);
		return {new_node, res.second};
	}
}

std::shared_ptr<const persistent_set::node> persistent_set::node::erase(
		const value_type & key) const {
	assert(this != nullptr);
	if (this->data == key) {
		if (this->left == nullptr || this->right == nullptr) {
			if (this->left == nullptr) {
				return this->right;
			} else {
				return this->left;
			}
		} else {
			auto next = this->right->find_first();
			value_type change = next->data;
			auto res = this->right->erase(change);
			auto new_node = std::make_shared<persistent_set::node>(change,
					this->left, res);
			return new_node;
			//hard
		}
	} else {
		if (key < this->data) {
			auto res = this->left->erase(key);
			auto new_node = std::make_shared<persistent_set::node>(this->data,
					res, this->right);
			return new_node;
		} else {
			auto res = this->right->erase(key);
			auto new_node = std::make_shared<persistent_set::node>(this->data,
					this->left, res);
			return new_node;
		}
	}
}

persistent_set::root_node::root_node(std::shared_ptr<const node> root) :
		root(root) {
}

persistent_set::iterator::iterator(std::weak_ptr<root_node> root,
		std::weak_ptr<const node> data) :
		root(root), data(data) {
}

const persistent_set::value_type & persistent_set::iterator::operator *() const {
	assert(this->data.expired() == false);
	assert(this->root.expired() == false);
	assert(this->root.lock() != nullptr);
	assert(this->data.lock() != nullptr);
	return this->data.lock()->data;
}

persistent_set::iterator & persistent_set::iterator::operator ++() {
	assert(this->data.expired() == false);
	assert(this->root.expired() == false);
	assert(this->root.lock() != nullptr);
	assert(this->data.lock() != nullptr);
	if (this->data.lock() == endnode) {
		this->data.reset();
	} else {
		auto next_node = this->root.lock()->root->find_next(
				this->data.lock()->data);
		*this = iterator(this->root, next_node);
	}
	return *this;
}

persistent_set::iterator persistent_set::iterator::operator ++(int) {
	iterator prev = *this;
	this->operator ++();
	return prev;
}

persistent_set::iterator & persistent_set::iterator::operator --() {
	assert(this->data.expired() == false);
	assert(this->root.expired() == false);
	assert(this->root.lock() != nullptr);
	assert(this->data.lock() != nullptr);
	if (this->data.lock() == endnode) {
		auto prev_node = this->root.lock()->root->find_last();
		if (prev_node == endnode) {
			this->data.reset();
		} else {
			*this = iterator(this->root, prev_node);
		}
	} else {
		auto prev_node = this->root.lock()->root->find_prev(
				this->data.lock()->data);
		if (prev_node == endnode) {
			this->data.reset();
		} else {
			*this = iterator(this->root, prev_node);
		}
	}
	return *this;
}

persistent_set::iterator persistent_set::iterator::operator --(int) {
	iterator prev = *this;
	this->operator --();
	return prev;
}

void persistent_set::printset() const {
	fprintf(stderr, "printing tree with addr %p\n", this->root.get());
	root->root->print_tree();
	fprintf(stderr, "\n");
}

void persistent_set::node::print_tree() const {
	if (this == nullptr) {
		return;
	}
	this->left->print_tree();
	fprintf(stderr, "value %d, addr %p, left addr %p, right addr %p\n",
			this->data, this, this->left.get(), this->right.get());
	this->right->print_tree();
}
