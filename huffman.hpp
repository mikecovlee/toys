#pragma once
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#ifdef __DEBUG__
#include <cstdio>
#define LOG(msg) ::printf("%s: %s\n", __TIME__, msg)
#else
#define LOG(msg)
#endif
class huffman_compress final {
public:
	huffman_compress() = delete;

private:
	// Binary Tree
	template <typename T>
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

			template <typename... Args_T>
			tree_node(tree_node *a, tree_node *b, tree_node *c, Args_T &&... args) : root(a), left(b), right(c),
				data(std::forward<Args_T>(args)...) {}

			~tree_node() = default;
		};

		static tree_node *copy(tree_node *raw, tree_node *root = nullptr)
		{
			if (raw == nullptr)
				return nullptr;
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
					throw std::runtime_error("E000E");
				return this->mData->data;
			}

			const T &data() const
			{
				if (!this->usable())
					throw std::runtime_error("E000E");
				return this->mData->data;
			}

			iterator root() const
			{
				if (!this->usable())
					throw std::runtime_error("E000E");
				return this->mData->root;
			}

			iterator left() const
			{
				if (!this->usable())
					throw std::runtime_error("E000E");
				return this->mData->left;
			}

			iterator right() const
			{
				if (!this->usable())
					throw std::runtime_error("E000E");
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

		tree_type(tree_type &&t) noexcept : mRoot(nullptr)
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
				throw std::runtime_error("E000E");
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
				throw std::runtime_error("E000E");
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
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData, it.mData->left, nullptr, data);
			if (it.mData->left != nullptr)
				it.mData->left->root = node;
			it.mData->left = node;
			return node;
		}

		iterator insert_left_right(iterator it, const T &data)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData, nullptr, it.mData->left, data);
			if (it.mData->left != nullptr)
				it.mData->left->root = node;
			it.mData->left = node;
			return node;
		}

		iterator insert_right_left(iterator it, const T &data)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData, it.mData->right, nullptr, data);
			if (it.mData->right != nullptr)
				it.mData->right->root = node;
			it.mData->right = node;
			return node;
		}

		iterator insert_right_right(iterator it, const T &data)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData, nullptr, it.mData->right, data);
			if (it.mData->right != nullptr)
				it.mData->right->root = node;
			it.mData->right = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_root_left(iterator it, Args &&... args)
		{
			if (it.mData == mRoot) {
				mRoot = new tree_node(nullptr, mRoot, nullptr, std::forward<Args>(args)...);
				return mRoot;
			}
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData->root, it.mData, nullptr, std::forward<Args>(args)...);
			if (it.mData->root->left == it.mData)
				it.mData->root->left = node;
			else
				it.mData->root->right = node;
			it.mData->root = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_root_right(iterator it, Args &&... args)
		{
			if (it.mData == mRoot) {
				mRoot = new tree_node(nullptr, nullptr, mRoot, std::forward<Args>(args)...);
				return mRoot;
			}
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData->root, nullptr, it.mData, std::forward<Args>(args)...);
			if (it.mData->root->left == it.mData)
				it.mData->root->left = node;
			else
				it.mData->root->right = node;
			it.mData->root = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_left_left(iterator it, Args &&... args)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData, it.mData->left, nullptr, std::forward<Args>(args)...);
			if (it.mData->left != nullptr)
				it.mData->left->root = node;
			it.mData->left = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_left_right(iterator it, Args &&... args)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData, nullptr, it.mData->left, std::forward<Args>(args)...);
			if (it.mData->left != nullptr)
				it.mData->left->root = node;
			it.mData->left = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_right_left(iterator it, Args &&... args)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData, it.mData->right, nullptr, std::forward<Args>(args)...);
			if (it.mData->right != nullptr)
				it.mData->right->root = node;
			it.mData->right = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_right_right(iterator it, Args &&... args)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *node = new tree_node(it.mData, nullptr, it.mData->right, std::forward<Args>(args)...);
			if (it.mData->right != nullptr)
				it.mData->right->root = node;
			it.mData->right = node;
			return node;
		}

		iterator erase(iterator it)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
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
				throw std::runtime_error("E000E");
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
				throw std::runtime_error("E000E");
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
				throw std::runtime_error("E000E");
			destroy(it.mData->left);
			it.mData->left = nullptr;
			return it;
		}

		iterator erase_right(iterator it)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			destroy(it.mData->right);
			it.mData->right = nullptr;
			return it;
		}

		iterator merge(iterator it, const tree_type<T> &tree)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
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
				throw std::runtime_error("E000E");
			tree_node *root = it.mData;
			tree_node *subroot = copy(tree.mRoot, root);
			destroy(root->left);
			root->left = subroot;
			return subroot;
		}

		iterator merge_right(iterator it, const tree_type<T> &tree)
		{
			if (!it.usable())
				throw std::runtime_error("E000E");
			tree_node *root = it.mData;
			tree_node *subroot = copy(tree.mRoot, root);
			destroy(root->right);
			root->right = subroot;
			return subroot;
		}
	};
	// Huffman Dictionary
	class encode_dict final {
		struct node_t {
			char ch = '\0';
			std::size_t freq = 0;
			node_t(char c, std::size_t f) : ch(c), freq(f) {}
		};
		std::unordered_map<char, std::string> dict;
		tree_type<node_t> tree;
		void gen_path(tree_type<node_t>::iterator it, std::string path)
		{
			if (!it.left().usable() && !it.right().usable()) {
				dict.emplace(it.data().ch, path);
				return;
			}
			gen_path(it.left(), path + "0");
			gen_path(it.right(), path + "1");
		}

	public:
		encode_dict() = delete;
		encode_dict(const encode_dict &) = delete;
		encode_dict(const std::vector<char> &buff)
		{
			std::unordered_map<char, std::size_t> freq;
			for (auto ch : buff) {
				if (freq.count(ch) == 0)
					freq.insert({ch, 1});
				else
					++freq[ch];
			}
			std::list<tree_type<node_t>> list;
			for (auto &it : freq) {
				list.emplace_back();
				list.back().emplace_root_left(list.back().root(), it.first, it.second);
			}
			while (list.size() > 1) {
				list.sort([](const tree_type<node_t> &lhs, const tree_type<node_t> &rhs) {
					return lhs.root().data().freq < rhs.root().data().freq;
				});
				tree_type<node_t> lhs(std::move(list.front()));
				list.pop_front();
				tree_type<node_t> rhs(std::move(list.front()));
				list.pop_front();
				lhs.emplace_root_left(lhs.root(), '\0', lhs.root().data().freq + rhs.root().data().freq);
				lhs.merge_right(lhs.root(), rhs);
				list.emplace_front(std::move(lhs));
			}
			tree = std::move(list.front());
		}
		std::unordered_map<char, std::string> operator()()
		{
			gen_path(tree.root(), "");
			return std::move(dict);
		}
	};
	// Internal Functions
	static std::unordered_map<std::string, char> decode_dict(const std::unordered_map<char, std::string> &encode)
	{
		std::unordered_map<std::string, char> decode;
		for (auto &it : encode)
			decode.emplace(it.second, it.first);
		return std::move(decode);
	}
	static char str2bin(std::string str)
	{
		if (str.size() != 8)
			throw std::runtime_error("Wrong literal.");
		char bin = 0x00;
		for (int i = 0; i < 8; ++i)
			if (str[i] == '1')
				bin |= 0x80 >> i;
		return bin;
	}
	static std::string bin2str(char bin)
	{
		std::string str(8, '0');
		for (int i = 0; i < 8; ++i)
			if (bin >> i & 0x01)
				str[7 - i] = '1';
		return std::move(str);
	}
	template <typename T>
	static void write_data(std::ostream &o, T dat)
	{
		char *ptr = reinterpret_cast<char *>(&dat);
		for (std::size_t i = 0; i < sizeof(T); ++i)
			o.put(*(ptr + i));
	}
	static void write_data(std::ostream &o, const std::vector<char> &buff)
	{
		for (std::size_t i = 0; i < buff.size(); i += 8) {
			char bin = 0x00;
			for (int idx = 0; idx < 8; ++idx)
				if (buff[i + idx] == '1')
					bin |= 0x80 >> idx;
			o.put(bin);
		}
	}
	template <typename T>
	static T read_data(std::istream &is)
	{
		char buff[sizeof(T)];
		for (std::size_t i = 0; i < sizeof(T); ++i)
			buff[i] = is.get();
		T dat(*reinterpret_cast<T *>(buff));
		return std::move(dat);
	}
	static int align_data(std::vector<char> &buff)
	{
		int align_size = buff.size() % 8;
		if (align_size == 0)
			return 0;
		align_size = 8 - align_size;
		for (int i = 0; i < align_size; ++i)
			buff.push_back('0');
		return align_size;
	}

public:
	/**
		 * File Structure
		 * Header: Dictionary Index Count(uint8_t)
		 * Dictionary Header: Character(int8_t), BytesofData(uint16_t)
		 * Dictionary Data:   Size(uint32_t), Align(uint8_t), Data
		 * File Data:         Size(uint64_t), Align(uint8_t), Data
		*/
	static void compress(const std::string &in, const std::string &out)
	{
		std::vector<char> buff;
		std::ifstream ifs(in, std::ios_base::binary);
        if (!ifs)
            throw std::runtime_error("File not exist.");
		LOG("Reading file context...");
		for (char ch;;) {
			ch = ifs.get();
			if (!ifs)
				break;
			buff.push_back(ch);
		}
		ifs.close();
		LOG("Generating dictionary...");
		auto encode = encode_dict(buff)();
		{
			LOG("Encoding...");
			std::vector<char> encode_buff;
			for (char ch : buff) {
				std::string code(encode[ch]);
				for (char c : code)
					encode_buff.push_back(c);
			}
			std::swap(buff, encode_buff);
		}
		int align = align_data(buff);
		std::ofstream ofs(out, std::ios_base::binary);
		LOG("Writing file header...");
		write_data<std::uint8_t>(ofs, encode.size());
		for (auto &it : encode) {
			write_data<std::int8_t>(ofs, it.first);
			write_data<std::uint16_t>(ofs, it.second.size());
		}
		LOG("Writing dictionary data...");
		std::vector<char> dict_buff;
		for (auto &it : encode) {
			for (auto &ch : it.second)
				dict_buff.push_back(ch);
		}
		int dict_align = align_data(dict_buff);
		write_data<std::uint32_t>(ofs, dict_buff.size() / 8);
		write_data<std::uint8_t>(ofs, dict_align);
		write_data(ofs, dict_buff);
		LOG("Writing File Data...");
		write_data<std::uint64_t>(ofs, buff.size() / 8);
		write_data<std::uint8_t>(ofs, align);
		write_data(ofs, buff);
	}
	static void decompress(const std::string &in, const std::string &out)
	{
		std::ifstream ifs(in, std::ios_base::binary);
        if (!ifs)
            throw std::runtime_error("File not exist.");
		LOG("Reading file header...");
		std::size_t count = read_data<std::uint8_t>(ifs);
		std::vector<std::pair<char, std::size_t>> index;
		for (std::size_t i = 0; i < count; ++i) {
			char ch = read_data<std::int8_t>(ifs);
			std::size_t size = read_data<std::uint16_t>(ifs);
			index.emplace_back(ch, size);
		}
		std::vector<char> buff;
		{
			LOG("Reading dictionary data...");
			std::size_t data_size = read_data<std::uint32_t>(ifs);
			int align = read_data<std::uint8_t>(ifs);
			for (std::size_t i = 0; i < data_size; ++i)
				buff.push_back(ifs.get());
			std::vector<char> encode_buff;
			for (char ch : buff)
				for (int i = 7; i >= 0; --i)
					encode_buff.push_back(ch >> i & 0x01 ? '1' : '0');
			for (int i = 0; i < align; ++i)
				encode_buff.pop_back();
			std::swap(buff, encode_buff);
		}
		std::unordered_map<std::string, char> decode;
		{
			LOG("Decoding dictionary data...");
			std::unordered_map<char, std::string> encode;
			std::size_t idx = 0;
			for (auto &it : index) {
				std::string code;
				for (std::size_t i = 0; i < it.second; ++i, ++idx)
					code.push_back(buff[idx]);
				encode.insert({it.first, code});
			}
			decode = decode_dict(encode);
		}
		buff.clear();
		{
			LOG("Reading file data...");
			std::size_t data_size = read_data<std::uint64_t>(ifs);
			int align = read_data<std::uint8_t>(ifs);
			for (std::size_t i = 0; i < data_size; ++i)
				buff.push_back(ifs.get());
			std::vector<char> encode_buff;
			for (char ch : buff)
				for (int i = 7; i >= 0; --i)
					encode_buff.push_back(ch >> i & 0x01 ? '1' : '0');
			for (int i = 0; i < align; ++i)
				encode_buff.pop_back();
			std::swap(buff, encode_buff);
		}
		ifs.close();
		std::ofstream ofs(out, std::ios_base::binary);
		std::string key;
		LOG("Decoding file data...");
		for (char ch : buff) {
			key.push_back(ch);
			if (decode.count(key) > 0) {
				ofs.put(decode[key]);
				key.clear();
			}
		}
	}
};