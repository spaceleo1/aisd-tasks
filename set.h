#ifndef UNTITLED_SET_H
#define UNTITLED_SET_H

#include <memory>
#include <stack>

template<typename ValueType>
class Set {
public:
    Set() : _root(nullptr), _sz(0) {}

    template<typename InputIterator>
    Set(InputIterator first, InputIterator last) : _root(nullptr), _sz(0) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    Set(std::initializer_list<ValueType> l) : _root(nullptr), _sz(0) {
        for (const ValueType &x: l) {
            insert(x);
        }
    }

    Set(const Set<ValueType> &s) : _root(nullptr), _sz(0) {
        for (const ValueType &x: s) {
            insert(x);
        }
    }

    Set<ValueType> &operator=(const Set<ValueType> &s) {
        if (s._root == _root) {
            return (*this);
        }
        _root = nullptr;
        _sz = 0;

        for (const ValueType &x: s) {
            insert(x);
        }

        return (*this);
    }

    size_t size() const {
        return _sz;
    }

    bool empty() const {
        return _root == nullptr;
    }

    void insert(const ValueType &x) {
        _root = insert(_root, x);
    }

    void erase(const ValueType &x) {
        if (find(x) != end()) --_sz;
        _root = erase(_root, x);
    }

private:
    struct Node {
        ValueType key;
        int height;
        std::shared_ptr<Node> left, right;

        Node(const ValueType &key) : key(key), height(1), left(nullptr), right(nullptr) {}

        ~Node() = default;
    };

    using __node_ptr = std::shared_ptr<Node>;

    __node_ptr _root;
    size_t _sz;

    __node_ptr RightRotate(__node_ptr y) {
        __node_ptr x = y->left;
        __node_ptr T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = std::max(GetHeight(y->left),
                             GetHeight(y->right)) + 1;
        x->height = std::max(GetHeight(x->left),
                             GetHeight(x->right)) + 1;

        return x;
    }

    __node_ptr LeftRotate(__node_ptr x) {
        __node_ptr y = x->right;
        __node_ptr T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = std::max(GetHeight(x->left),
                             GetHeight(x->right)) + 1;
        y->height = std::max(GetHeight(y->left),
                             GetHeight(y->right)) + 1;

        return y;
    }

    int GetHeight(__node_ptr N) {
        if (N == nullptr) {
            return 0;
        }
        return N->height;
    }

    int GetBalance(__node_ptr N) {
        if (N == nullptr) {
            return 0;
        }
        return GetHeight(N->left) - GetHeight(N->right);
    }

    __node_ptr insert(__node_ptr node, ValueType key) {
        if (node == nullptr) {
            __node_ptr a = std::make_shared<Node>(key);
            ++_sz;
            return a;
        }

        if (key < node->key) {
            node->left = insert(node->left, key);
        } else if (node->key < key) {
            node->right = insert(node->right, key);
        } else {
            return node;
        }

        node->height = 1 + std::max(GetHeight(node->left),
                                    GetHeight(node->right));

        int balance = GetBalance(node);

        if (balance > 1 && key < node->left->key) {
            return RightRotate(node);
        }

        if (balance < -1 && node->right->key < key) {
            return LeftRotate(node);
        }

        if (balance > 1 && node->left->key < key) {
            node->left = LeftRotate(node->left);
            return RightRotate(node);
        }

        if (balance < -1 && key < node->right->key) {
            node->right = RightRotate(node->right);
            return LeftRotate(node);
        }

        return node;
    }

    __node_ptr MinValueNode(__node_ptr node) {
        __node_ptr tmp = node;

        while (tmp->left != nullptr)
            tmp = tmp->left;

        return tmp;
    }

    __node_ptr erase(__node_ptr node, const ValueType& key) {
        if (node == nullptr) {
            return node;
        }

        if (key < node->key) {
            node->left = erase(node->left, key);
        } else if (node->key < key) {
            node->right = erase(node->right, key);
        } else {
            if (node->left == nullptr || node->right == nullptr) {
                __node_ptr tmp = node->left != nullptr ? node->left : node->right;

                if (tmp == nullptr) {
                    node = nullptr;
                } else {
                    *node = *tmp;
                }
            } else {
                __node_ptr tmp = MinValueNode(node->right);
                node->key = tmp->key;
                node->right = erase(node->right, tmp->key);
            }
        }

        if (node == nullptr) {
            return node;
        }

        node->height = 1 + std::max(GetHeight(node->left),
                                    GetHeight(node->right));

        int balance = GetBalance(node);

        if (balance > 1 && GetBalance(node->left) >= 0) {
            return RightRotate(node);
        }

        if (balance > 1 && GetBalance(node->left) < 0) {
            node->left = LeftRotate(node->left);
            return RightRotate(node);
        }

        if (balance < -1 && GetBalance(node->right) <= 0) {
            return LeftRotate(node);
        }

        if (balance < -1 && GetBalance(node->right) > 0) {
            node->right = RightRotate(node->right);
            return LeftRotate(node);
        }

        return node;
    }

public:
    class iterator {
    public:
        iterator() {}

        iterator(__node_ptr root) : _st(std::stack<__node_ptr>()), _root(root) {}

        iterator(__node_ptr root, const ValueType& x) : _st(std::stack<__node_ptr>()), _root(root)  {
            while (root != nullptr) {
                _st.push(root);

                if (root->key < x) {
                    root = root->right;
                } else {
                    root = root->left;
                }
            }

            while (!_st.empty() && _st.top()->key < x) {
                _st.pop();
            }
        }

        iterator(const iterator &it) {
            _st = it._st;
            _root = it._root;
        }

        ~iterator() {}

        iterator &operator=(const iterator &it) {
            _st = it._st;
            _root = it._root;

            return *this;
        }

        iterator &operator++() {
            __node_ptr tmp = _st.top()->right;
            if (tmp != nullptr) {
                PushLeft(tmp);
            } else {
                tmp = _st.top();
                _st.pop();
                while (!_st.empty() && _st.top()->right == tmp) {
                    tmp = _st.top();
                    _st.pop();
                }
            }

            return (*this);
        }

        iterator operator++(int) {
            iterator it = *this;

            __node_ptr tmp = _st.top()->right;
            if (tmp != nullptr) {
                PushLeft(tmp);
            } else {
                tmp = _st.top();
                _st.pop();
                while (!_st.empty() && _st.top()->right == tmp) {
                    tmp = _st.top();
                    _st.pop();
                }
            }

            return it;
        }

        iterator &operator--() {
            if (_st.empty()) {
                PushRight(_root);
            } else {
                __node_ptr tmp = _st.top()->left;
                if (tmp != nullptr) {
                    PushRight(tmp);
                } else {
                    tmp = _st.top();
                    _st.pop();
                    while (!_st.empty() && _st.top()->left == tmp) {
                        tmp = _st.top();
                        _st.pop();
                    }
                }
            }

            return (*this);
        }

        iterator operator--(int) {
            iterator it = *this;
            if (_st.empty()) {
                PushRight(_root);
            } else {
                __node_ptr tmp = _st.top()->left;
                if (tmp != nullptr) {
                    PushRight(tmp);
                } else {
                    tmp = _st.top();
                    _st.pop();
                    while (!_st.empty() && _st.top()->left == tmp) {
                        tmp = _st.top();
                        _st.pop();
                    }
                }
            }

            return it;
        }

        const ValueType &operator*() const {
            return _st.top()->key;
        }

        ValueType const *operator->() const {
            return &_st.top()->key;
        }

        friend bool operator==(const iterator &it1, const iterator &it2) {
            if (it1._st.size() != it2._st.size()) return false;
            return it1._st.empty() || it1._st.top() == it2._st.top();
        }

        friend bool operator!=(const iterator &it1, const iterator &it2) {
            if (it1._st.size() != it2._st.size()) return true;
            return !it1._st.empty() && it1._st.top() != it2._st.top();
        }

        void PushLeft(__node_ptr root) {
            while (root != nullptr) {
                _st.push(root);
                root = root->left;
            }
        }

        void PushRight(__node_ptr root) {
            while (root != nullptr) {
                _st.push(root);
                root = root->right;
            }
        }

    private:
        std::stack<__node_ptr> _st;
        __node_ptr _root;
    };

    iterator begin() const {
        iterator it(_root);
        it.PushLeft(_root);
        return it;
    }

    iterator end() const {
        return iterator(_root);
    }

    iterator find(const ValueType& x) const {
        iterator it = iterator(_root, x);
        if (it == end() || *it < x || x < *it) return end();
        return it;
    }

    iterator lower_bound(const ValueType& x) const {
        return iterator(_root, x);
    }
};

#endif //UNTITLED_SET_H
