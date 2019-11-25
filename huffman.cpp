#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <list>
namespace cov {
    using error = std::runtime_error;
}
// Binary Tree
template<typename T>
class tree_type final {
	struct tree_node final {
		tree_node *root = nullptr;
		tree_node *left = nullptr;
		tree_node *right = nullptr;
		T data;

		tree_node() = default;

		tree_node(const tree_node &) = default;

		tree_node(tree_node &&) noexcept = default;

		tree_node(tree_node *a, tree_node *b, tree_node *c, const T &dat) : root(a), left(b), right(c), data(dat) {}

		template<typename...Args_T>
		tree_node(tree_node *a, tree_node *b, tree_node *c, Args_T &&...args):root(a), left(b), right(c),
			data(std::forward<Args_T>(args)...) {}

		~tree_node() = default;
	};

	static tree_node *copy(tree_node *raw, tree_node *root = nullptr)
	{
		if (raw == nullptr) return nullptr;
		tree_node *node = new tree_node(root, nullptr, nullptr, raw->data);
		node->left = copy(raw->left, node);
		node->right = copy(raw->right, node);
		return node;
	}

	static void destroy(tree_node *raw)
	{
		if (raw != nullptr) {
			destroy(raw->left);
			destroy(raw->right);
			delete raw;
		}
	}

	tree_node *mRoot = nullptr;
public:
	class iterator final {
		friend class tree_type;

		tree_node *mData = nullptr;
	public:
		iterator() = default;

		iterator(tree_node *ptr) : mData(ptr) {}

		iterator(const iterator &) = default;

		iterator(iterator &&) noexcept = default;

		~iterator() = default;

		iterator &operator=(const iterator &) = default;

		iterator &operator=(iterator &&) noexcept = default;

		bool usable() const noexcept
		{
			return this->mData != nullptr;
		}

		T &data()
		{
			if (!this->usable())
				throw cov::error("E000E");
			return this->mData->data;
		}

		const T &data() const
		{
			if (!this->usable())
				throw cov::error("E000E");
			return this->mData->data;
		}

		iterator root() const
		{
			if (!this->usable())
				throw cov::error("E000E");
			return this->mData->root;
		}

		iterator left() const
		{
			if (!this->usable())
				throw cov::error("E000E");
			return this->mData->left;
		}

		iterator right() const
		{
			if (!this->usable())
				throw cov::error("E000E");
			return this->mData->right;
		}
	};

	void swap(tree_type &t)
	{
		tree_node *ptr = this->mRoot;
		this->mRoot = t.mRoot;
		t.mRoot = ptr;
	}

	void swap(tree_type &&t) noexcept
	{
		tree_node *ptr = this->mRoot;
		this->mRoot = t.mRoot;
		t.mRoot = ptr;
	}

	tree_type() = default;

	explicit tree_type(iterator it) : mRoot(copy(it.mData)) {}

	tree_type(const tree_type &t) : mRoot(copy(t.mRoot)) {}

	tree_type(tree_type &&t) noexcept:
		mRoot(nullptr)
	{
		swap(t);
	}

	~tree_type()
	{
		destroy(this->mRoot);
	}

	tree_type &operator=(const tree_type &t)
	{
		if (&t != this) {
			destroy(this->mRoot);
			this->mRoot = copy(t.mRoot);
		}
		return *this;
	}

	tree_type &operator=(tree_type &&t) noexcept
	{
		swap(t);
		return *this;
	}

	void assign(const tree_type &t)
	{
		if (&t != this) {
			destroy(this->mRoot);
			this->mRoot = copy(t.mRoot);
		}
	}

	bool empty() const noexcept
	{
		return this->mRoot == nullptr;
	}

	void clear()
	{
		destroy(this->mRoot);
		this->mRoot = nullptr;
	}

	iterator root()
	{
		return this->mRoot;
	}

	iterator root() const
	{
		return this->mRoot;
	}

	iterator insert_root_left(iterator it, const T &data)
	{
		if (it.mData == mRoot) {
			mRoot = new tree_node(nullptr, mRoot, nullptr, data);
			return mRoot;
		}
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData->root, it.mData, nullptr, data);
		if (it.mData->root->left == it.mData)
			it.mData->root->left = node;
		else
			it.mData->root->right = node;
		it.mData->root = node;
		return node;
	}

	iterator insert_root_right(iterator it, const T &data)
	{
		if (it.mData == mRoot) {
			mRoot = new tree_node(nullptr, nullptr, mRoot, data);
			return mRoot;
		}
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData->root, nullptr, it.mData, data);
		if (it.mData->root->left == it.mData)
			it.mData->root->left = node;
		else
			it.mData->root->right = node;
		it.mData->root = node;
		return node;
	}

	iterator insert_left_left(iterator it, const T &data)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData, it.mData->left, nullptr, data);
		if (it.mData->left != nullptr)
			it.mData->left->root = node;
		it.mData->left = node;
		return node;
	}

	iterator insert_left_right(iterator it, const T &data)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData, nullptr, it.mData->left, data);
		if (it.mData->left != nullptr)
			it.mData->left->root = node;
		it.mData->left = node;
		return node;
	}

	iterator insert_right_left(iterator it, const T &data)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData, it.mData->right, nullptr, data);
		if (it.mData->right != nullptr)
			it.mData->right->root = node;
		it.mData->right = node;
		return node;
	}

	iterator insert_right_right(iterator it, const T &data)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData, nullptr, it.mData->right, data);
		if (it.mData->right != nullptr)
			it.mData->right->root = node;
		it.mData->right = node;
		return node;
	}

	template<typename...Args>
	iterator emplace_root_left(iterator it, Args &&...args)
	{
		if (it.mData == mRoot) {
			mRoot = new tree_node(nullptr, mRoot, nullptr, std::forward<Args>(args)...);
			return mRoot;
		}
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData->root, it.mData, nullptr, std::forward<Args>(args)...);
		if (it.mData->root->left == it.mData)
			it.mData->root->left = node;
		else
			it.mData->root->right = node;
		it.mData->root = node;
		return node;
	}

	template<typename...Args>
	iterator emplace_root_right(iterator it, Args &&...args)
	{
		if (it.mData == mRoot) {
			mRoot = new tree_node(nullptr, nullptr, mRoot, std::forward<Args>(args)...);
			return mRoot;
		}
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData->root, nullptr, it.mData, std::forward<Args>(args)...);
		if (it.mData->root->left == it.mData)
			it.mData->root->left = node;
		else
			it.mData->root->right = node;
		it.mData->root = node;
		return node;
	}

	template<typename...Args>
	iterator emplace_left_left(iterator it, Args &&...args)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData, it.mData->left, nullptr, std::forward<Args>(args)...);
		if (it.mData->left != nullptr)
			it.mData->left->root = node;
		it.mData->left = node;
		return node;
	}

	template<typename...Args>
	iterator emplace_left_right(iterator it, Args &&...args)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData, nullptr, it.mData->left, std::forward<Args>(args)...);
		if (it.mData->left != nullptr)
			it.mData->left->root = node;
		it.mData->left = node;
		return node;
	}

	template<typename...Args>
	iterator emplace_right_left(iterator it, Args &&...args)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData, it.mData->right, nullptr, std::forward<Args>(args)...);
		if (it.mData->right != nullptr)
			it.mData->right->root = node;
		it.mData->right = node;
		return node;
	}

	template<typename...Args>
	iterator emplace_right_right(iterator it, Args &&...args)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *node = new tree_node(it.mData, nullptr, it.mData->right, std::forward<Args>(args)...);
		if (it.mData->right != nullptr)
			it.mData->right->root = node;
		it.mData->right = node;
		return node;
	}

	iterator erase(iterator it)
	{
		if (!it.usable())
			throw cov::error("E000E");
		if (it.mData == mRoot) {
			destroy(mRoot);
			mRoot = nullptr;
			return nullptr;
		}
		tree_node *root = it.mData->root;
		if (root != nullptr) {
			if (it.mData == root->left)
				root->left = nullptr;
			else
				root->right = nullptr;
		}
		destroy(it.mData);
		return root;
	}

	iterator reserve_left(iterator it)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *reserve = it.mData->left;
		tree_node *root = it.mData->root;
		it.mData->left = nullptr;
		reserve->root = root;
		if (root != nullptr) {
			if (it.mData == root->left)
				root->left = reserve;
			else
				root->right = reserve;
		}
		destroy(it.mData);
		if (it.mData == mRoot)
			mRoot = reserve;
		return reserve;
	}

	iterator reserve_right(iterator it)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *reserve = it.mData->right;
		tree_node *root = it.mData->root;
		it.mData->right = nullptr;
		reserve->root = root;
		if (root != nullptr) {
			if (it.mData == root->left)
				root->left = reserve;
			else
				root->right = reserve;
		}
		destroy(it.mData);
		if (it.mData == mRoot)
			mRoot = reserve;
		return reserve;
	}

	iterator erase_left(iterator it)
	{
		if (!it.usable())
			throw cov::error("E000E");
		destroy(it.mData->left);
		it.mData->left = nullptr;
		return it;
	}

	iterator erase_right(iterator it)
	{
		if (!it.usable())
			throw cov::error("E000E");
		destroy(it.mData->right);
		it.mData->right = nullptr;
		return it;
	}

	iterator merge(iterator it, const tree_type<T> &tree)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *root = it.mData->root;
		tree_node *subroot = copy(tree.mRoot, root);
		if (root != nullptr) {
			if (it.mData == root->left)
				root->left = subroot;
			else
				root->right = subroot;
		}
		destroy(it.mData);
		if (it.mData == mRoot)
			mRoot = subroot;
		return subroot;
	}

    iterator merge_left(iterator it, const tree_type<T> &tree)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *root = it.mData;
		tree_node *subroot = copy(tree.mRoot, root);
		destroy(root->left);
        root->left = subroot;
		return subroot;
	}

    iterator merge_right(iterator it, const tree_type<T> &tree)
	{
		if (!it.usable())
			throw cov::error("E000E");
		tree_node *root = it.mData;
		tree_node *subroot = copy(tree.mRoot, root);
		destroy(root->right);
        root->right = subroot;
		return subroot;
	}
};
class huffman_encoder final {
    struct node_t {
        char ch = '\0';
        std::size_t freq = 0;
        node_t(char c, std::size_t f) : ch(c), freq(f) {}
    };
    std::unordered_map<char, std::string> dict;
    tree_type<node_t> tree;
    void gen_path(tree_type<node_t>::iterator, std::string);
public:
    huffman_encoder() = delete;
    huffman_encoder(const huffman_encoder&) = delete;
    ~huffman_encoder() = default;
    huffman_encoder(const std::vector<char>&);
    std::unordered_map<char, std::string> operator()();
};
void huffman_encoder::gen_path(tree_type<node_t>::iterator it, std::string path)
{
    if(!it.left().usable()&&!it.right().usable())
    {
        dict.emplace(it.data().ch, path);
        return;
    }
    gen_path(it.left(), path + "0");
    gen_path(it.right(), path + "1");
}
huffman_encoder::huffman_encoder(const std::vector<char>& buff)
{
    std::unordered_map<char, std::size_t> freq;
    for (auto ch:buff)
    {
        if(freq.count(ch) == 0)
            freq.insert({ch, 1});
        else
            ++freq[ch];
    }
    std::list<tree_type<node_t>> list;
    for (auto& it:freq)
    {
        list.emplace_back();
        list.back().emplace_root_left(list.back().root(), it.first, it.second);
    }
    while (list.size() > 1)
    {
        list.sort([](const tree_type<node_t>& lhs, const tree_type<node_t>& rhs){ return lhs.root().data().freq < rhs.root().data().freq; });
        tree_type<node_t> lhs(std::move(list.front()));
        list.pop_front();
        tree_type<node_t> rhs(std::move(list.front()));
        list.pop_front();
        lhs.emplace_root_left(lhs.root(), '\0', lhs.root().data().freq + rhs.root().data().freq);
        lhs.merge_right(lhs.root(), rhs);
        std:list.emplace_front(std::move(lhs));
    }
    tree = std::move(list.front());
}
std::unordered_map<char, std::string> huffman_encoder::operator()()
{
    gen_path(tree.root(), "");
    return std::move(dict);
}
#include <fstream>
int main(int argc, char** argv)
{
    if (argc != 4)
        return -1;
    std::vector<char> buff;
    std::ifstream ifs(argv[1]);
    for(char ch;;)
    {
        ch = ifs.get();
        if (!ifs)
            break;
        buff.push_back(ch);
    }
    ifs.close();
    auto encode_dict = huffman_encoder(buff)();
    std::ofstream ofs(argv[2]);
    for (char ch:buff)
        ofs << encode_dict[ch];
    ofs.close();
    std::unordered_map<std::string, char> decode_dict;
    for (auto &it:encode_dict)
        decode_dict.emplace(it.second, it.first);
    ifs.open(argv[2]);
    ofs.open(argv[3]);
    std::string key;
    for(char ch;;)
    {
        ch = ifs.get();
        if (!ifs)
            break;
        key.push_back(ch);
        if (decode_dict.count(key) > 0)
        {
            ofs.put(decode_dict[key]);
            key.clear();
        }
    }
    return 0;
}