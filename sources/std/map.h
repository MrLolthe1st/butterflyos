#ifndef _MAP_H
#define _MAP_H (1)
#include "trees.h"


template < typename _Key, typename _Ty>
class pair {
public:
	_Key first = _Key();
	_Ty second = _Ty();
	pair<_Key, _Ty>& operator=(pair<_Key, _Ty>  a)
	{
		first = a.first;
		second = a.second;
        return *this;
	}

	int operator-(pair<_Key, _Ty> a)
	{
		if (this->first - a.first)
			return this->first - a.first;
		else return this->second - a.second;
	}
	pair() {};
};

template < typename _Key, typename _Ty>
int memcmp(struct rb_tree * w, struct rb_node * z, struct rb_node * x) {
	pair<_Key, _Ty> * a = (pair<_Key, _Ty> *)z->value;
	pair<_Key, _Ty> * b = (pair<_Key, _Ty> *)x->value;
	return a - b;
}

template < typename _Key, typename _Ty>
class map {
	struct rb_tree *tree;
public:
	class iterator : public pair<_Key, _Ty> {
	public:
		struct rb_iter * iter;

		iterator(struct rb_iter * it) { iter = it; };

		iterator& operator=(pair<_Key, _Ty>  a)
		{
			this->first = a.first;
			this->second = a.second;
            return *this;
		}

		iterator operator++(int a)
		{
			iterator T = *this;
			void * z;
			z = rb_iter_next(iter);
			pair<_Key, _Ty> * w = (pair<_Key, _Ty> *) z;
			if (!w)
			{
				rb_iter_dealloc(this->iter);
				this->iter = 0;
				return *this;
			}
			this->first = w->first;
			this->second = w->second;
			return T;
		}

		~iterator() { if (iter) rb_iter_dealloc(iter); }

		iterator& operator++()
		{
			void * z;
			z = rb_iter_next(iter);
			pair<_Key, _Ty> * w = (pair<_Key, _Ty> *) z;
			if (!w)
			{
				rb_iter_dealloc(this->iter);
				this->iter = 0;
				return *this;
			}
			this->first = w->first;
			this->second = w->second;
			return *this;
		}

		bool operator!=(iterator b)
		{
			return this->iter != b.iter;
		}

		bool operator==(iterator b)
		{
			return this->iter == b.iter;
		}
	};

	void clear()
	{
		rb_tree_node_cmp_f u = tree->cmp;
		rb_tree_dealloc(tree, NULL);
		tree = rb_tree_create(u);
	}

	iterator begin()
	{
		struct rb_iter *iter = rb_iter_create();
		pair<_Key, _Ty> * val = (pair<_Key, _Ty> *) rb_iter_first(iter, tree);
		iterator it = iterator(iter);
		it = *val;
		return it;
	}

	iterator end()
	{
		return _end;
	}

	int size() {
		return tree->size;
	}

	_Ty& operator[](_Key k) {
		pair<_Key, _Ty> * p = (pair<_Key, _Ty>*)malloc(sizeof(pair<_Key, _Ty>));
		p->first = k; p->second = _Ty();
		pair<_Key, _Ty> * r = (pair<_Key, _Ty> *) rb_tree_find(tree, p);
		if (!r) {
			rb_tree_insert(tree, p);
			return p->second;
		}
		else { free(p); return r->second; }
	}

	int erase(_Key k)
	{
		pair<_Key, _Ty> * p = (pair<_Key, _Ty>*)malloc(sizeof(pair<_Key, _Ty>));
		p->first = k; p->second = _Ty();
		int res = rb_tree_remove(tree, p);
		free(p);
		return res;
	}

	~map() { rb_tree_dealloc(tree, NULL); };

	map() { tree = rb_tree_create(memcmp<_Key, _Ty>); };

	void set_cmp(rb_tree_node_cmp_f z) { if (tree) rb_tree_dealloc(tree, NULL);  tree = rb_tree_create(z); };
private:
	iterator _end = iterator(0);
};
#endif